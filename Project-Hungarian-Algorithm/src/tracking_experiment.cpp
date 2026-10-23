// Consecutive-frame KITTI association. Solvers receive Box vectors only.
#include "tracking.hpp"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

namespace {
using Clock=std::chrono::steady_clock;
volatile lap::Cost sink=0;
struct Config{std::string metric,name;lap::Cost penalty;double distance_gate,iou_gate;};
std::vector<tracking::Box> geometry(const tracking::Frame&f){
    std::vector<tracking::Box> out;for(const auto&d:f.detections)out.push_back(d.box);return out;
}
tracking::Association solve(const std::vector<tracking::Box>&a,const std::vector<tracking::Box>&b,const Config&c,const std::string&method){
    return tracking::associate(a,b,c.metric,method,c.penalty,c.distance_gate,c.iou_gate);
}
void timing(std::ofstream&out,const std::string&seq,int frame,const std::vector<tracking::Box>&a,const std::vector<tracking::Box>&b,const Config&c,const std::string&method){
    auto start=Clock::now();auto warm=solve(a,b,c,method);sink+=warm.objective;
    double pilot=std::chrono::duration<double,std::micro>(Clock::now()-start).count();
    int batch=std::clamp(static_cast<int>(200.0/std::max(pilot,0.001)),1,64);
    for(int rep=0;rep<3;++rep){
        auto begin=Clock::now();
        for(int k=0;k<batch;++k){auto result=solve(a,b,c,method);sink+=result.objective;}
        double us=std::chrono::duration<double,std::micro>(Clock::now()-begin).count()/batch;
        out<<seq<<','<<frame<<','<<c.metric<<','<<method<<','<<a.size()<<','<<b.size()<<','<<rep<<','<<batch<<','<<us<<'\n';
    }
}
}
int main(int argc,char**argv){try{
    if(argc!=3)throw std::invalid_argument("Usage: lap_tracking_experiment LABEL_02_DIRECTORY OUTPUT_DIRECTORY");
    namespace fs=std::filesystem;fs::path labels=argv[1],dest=argv[2];fs::create_directories(dest);
    std::vector<fs::path> files;for(const auto&e:fs::directory_iterator(labels))if(e.path().extension()==".txt")files.push_back(e.path());
    std::sort(files.begin(),files.end());if(files.empty())throw std::runtime_error("No label files");
    std::ofstream pairs(dest/"kitti_pairs.csv"), times(dest/"kitti_timing.csv"), objects(dest/"kitti_objects.csv"), inventory(dest/"kitti_inventory.csv");
    if(!pairs||!times||!objects||!inventory)throw std::runtime_error("Cannot open result files");
    pairs<<"sequence,frame,metric,config,method,penalty,distance_gate,iou_gate,previous,current,eligible,gate_eligible,predicted,correct,incorrect,objective,fingerprint,certificate_ok\n";
    times<<"sequence,frame,metric,method,previous,current,repeat,batch,time_us\n";times<<std::setprecision(12);
    objects<<"sequence,frame,metric,method,detection_index,occlusion,truncation,height,crowding,eligible,predicted,correct\n";
    inventory<<"sequence,frames,pairs,car_boxes,car_frames,active_pairs,max_cars,max_augmented_order\n";
    std::vector<Config> configs;
    for(const std::string metric:{"distance","iou"}){
        configs.push_back({metric,"baseline",600000,80,0.05});
        configs.push_back({metric,"penalty_low",400000,80,0.05});
        configs.push_back({metric,"penalty_high",800000,80,0.05});
        configs.push_back({metric,"gate_low",600000,metric=="distance"?40.0:80.0,metric=="iou"?0.0:0.05});
        configs.push_back({metric,"gate_high",600000,metric=="distance"?160.0:80.0,metric=="iou"?0.2:0.05});
    }
    std::size_t record_count=0;
    for(const auto&file:files){
        auto frames=tracking::read_kitti(file.string());const auto sequence=file.stem().string();
        std::size_t boxes=0,car_frames=0,active=0,maxcars=0,maxq=0;
        for(const auto&f:frames){boxes+=f.detections.size();car_frames+=!f.detections.empty();maxcars=std::max(maxcars,f.detections.size());}
        for(std::size_t f=1;f<frames.size();++f){
            // Construct geometry and BOTH predictions before accessing evaluation IDs.
            auto previous=geometry(frames[f-1]),current=geometry(frames[f]);
            active+=!previous.empty()||!current.empty();maxq=std::max(maxq,previous.size()+current.size());
            for(const auto&cfg:configs){
                auto a=tracking::augmented_cost(previous,current,cfg.metric,cfg.penalty,cfg.distance_gate,cfg.iou_gate);
                auto h=solve(previous,current,cfg,"hungarian"),g=solve(previous,current,cfg,"greedy");
                auto certificate=lap::hungarian(a);
                if(!lap::valid_certificate(a,certificate)||certificate.cost!=h.objective||h.objective>g.objective)throw std::runtime_error("Certificate or objective comparison failed");
                const lap::Cost forbidden=static_cast<lap::Cost>(previous.size()+current.size())*cfg.penalty+1000001;
                // Only now do we inspect ground truth; no outcome returns to solve().
                std::map<int,std::size_t> previous_ids;
                for(std::size_t i=0;i<previous.size();++i)previous_ids.emplace(frames[f-1].detections[i].truth_id,i);
                int eligible=0,gate_eligible=0;
                for(std::size_t j=0;j<current.size();++j){auto it=previous_ids.find(frames[f].detections[j].truth_id);
                    if(it!=previous_ids.end()){++eligible;gate_eligible+=a(it->second,j)!=forbidden;}}
                for(const std::string method:{"hungarian","greedy"}){
                    const auto&result=method=="hungarian"?h:g;
                    std::vector<int> incoming(current.size(),-1);int predicted=0,correct=0;lap::Cost recomputed=static_cast<lap::Cost>(previous.size()+current.size())*cfg.penalty;
                    for(std::size_t i=0;i<previous.size();++i){int j=result.detection_of_track[i];if(j<0)continue;
                        if(static_cast<std::size_t>(j)>=current.size()||incoming[j]>=0||a(i,j)==forbidden)throw std::runtime_error("Invalid optional link");
                        incoming[j]=static_cast<int>(i);++predicted;recomputed+=a(i,j)-2*cfg.penalty;
                        correct+=frames[f-1].detections[i].truth_id==frames[f].detections[j].truth_id;
                    }
                    if(recomputed!=result.objective)throw std::runtime_error("Optional objective mismatch");
                    pairs<<sequence<<','<<f<<','<<cfg.metric<<','<<cfg.name<<','<<method<<','<<cfg.penalty<<','<<cfg.distance_gate<<','<<cfg.iou_gate<<','<<previous.size()<<','<<current.size()<<','<<eligible<<','<<gate_eligible<<','<<predicted<<','<<correct<<','<<predicted-correct<<','<<result.objective<<','<<lap::fingerprint(a)<<",1\n";
                    ++record_count;
                    if(cfg.name=="baseline")for(std::size_t j=0;j<current.size();++j){const auto&d=frames[f].detections[j];int i=incoming[j];
                        objects<<sequence<<','<<f<<','<<cfg.metric<<','<<method<<','<<j<<','<<d.occlusion<<','<<d.truncation<<','<<d.box.height<<','<<std::max(previous.size(),current.size())<<','<<previous_ids.count(d.truth_id)<<','<<(i>=0)<<','<<(i>=0&&frames[f-1].detections[i].truth_id==d.truth_id)<<'\n';
                    }
                }
                if(cfg.name=="baseline"){
                    // Alternate order to reduce fixed-order timing bias.
                    if(f%2){timing(times,sequence,static_cast<int>(f),previous,current,cfg,"hungarian");timing(times,sequence,static_cast<int>(f),previous,current,cfg,"greedy");}
                    else{timing(times,sequence,static_cast<int>(f),previous,current,cfg,"greedy");timing(times,sequence,static_cast<int>(f),previous,current,cfg,"hungarian");}
                }
            }
        }
        inventory<<sequence<<','<<frames.size()<<','<<(frames.empty()?0:frames.size()-1)<<','<<boxes<<','<<car_frames<<','<<active<<','<<maxcars<<','<<maxq<<'\n';
        std::cout<<"Completed "<<sequence<<": "<<frames.size()<<" frames\n";
    }
    std::cout<<"PASS: "<<record_count<<" paired configuration/method records; all certificates and optional objectives verified.\n";
    return 0;
}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
