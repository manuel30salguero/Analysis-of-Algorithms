#include "tracking.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>

int main(int argc,char**argv){try{
    std::ofstream evidence;if(argc>1){evidence.open(argv[1]);if(!evidence)throw std::runtime_error("Cannot write optional CSV");evidence<<"instance,order,fingerprint,hungarian_cost,exhaustive_cost,certificate_ok\n";}
    int instances=0;
    auto oracle=[&](const lap::Matrix&a,lap::Cost value){
        auto exact=lap::exhaustive(a);bool cert=lap::valid_certificate(a,lap::hungarian(a));++instances;
        if(evidence)evidence<<instances<<','<<a.rows<<','<<lap::fingerprint(a)<<','<<value<<','<<exact.cost<<','<<cert<<'\n';
        return value==exact.cost&&cert;
    };
    int checks=0;auto check=[&](bool x){++checks;if(!x)throw std::runtime_error("Tracking assertion failed");};
    using tracking::Box;std::vector<Box>t={{0,0,10,10},{20,0,10,10}},d={{1,0,10,10},{21,0,10,10}};
    check(tracking::iou(t[0],t[0])==1.0);check(tracking::iou(t[0],t[1])==0.0);
    for(const std::string metric:{"distance","iou"}){
        auto a=tracking::augmented_cost(t,d,metric);
        check(oracle(a,lap::hungarian(a).cost));
        auto h=tracking::associate(t,d,metric,"hungarian");
        check(h.detection_of_track==std::vector<int>({0,1}));
        check(h.objective<=tracking::associate(t,d,metric,"greedy").objective);
        check(tracking::associate(t,{},metric,"hungarian").objective==1200000);
        check(tracking::associate({},d,metric,"hungarian").objective==1200000);
        check(tracking::associate({}, {},metric,"hungarian").objective==0);
    }
    auto far=tracking::associate(t,{{1000,0,10,10}},"distance","hungarian");
    check(far.detection_of_track==std::vector<int>({-1,-1}));check(far.objective==1800000);
    for(int s=0;s<80;++s){auto frames=tracking::synthetic_sequence(3,25,20261021+s,"ambiguous");
        for(int k=1;k<25;++k){std::vector<Box>x,y;
            for(auto b:frames[k-1].detections)x.push_back(b.box);
            for(auto b:frames[k].detections)y.push_back(b.box);
            for(const std::string metric:{"distance","iou"}){
                auto a=tracking::augmented_cost(x,y,metric);
                auto h=tracking::associate(x,y,metric,"hungarian");
                check(oracle(a,h.objective));
                check(h.objective<=tracking::associate(x,y,metric,"greedy").objective);
            }
        }
    }
    // A local miniature fixture verifies parsing, skipped classes and missing frames.
    const std::string path="test_kitti_fixture.txt";
    {std::ofstream o(path);o<<"0 3 Car 0 0 0 10 20 30 40 0 0 0 0 0 0 0\n"
      <<"2 7 Pedestrian 0 0 0 1 1 5 5 0 0 0 0 0 0 0\n"
      <<"2 3 Car 0 0 0 12 20 32 40 0 0 0 0 0 0 0\n";}
    auto frames=tracking::read_kitti(path);std::remove(path.c_str());
    check(frames.size()==3);check(frames[1].detections.empty());
    check(frames[0].detections[0].truth_id==3);check(frames[2].detections[0].box.cx==22);
    // Relabel and reorder annotations: assignments must depend on geometry only.
    {std::ofstream o(path);o<<"0 3 Car 0 0 0 10 20 30 40 0 0 0 0 0 0 0\n"
      <<"0 7 Car 2 3 0 50 20 70 40 0 0 0 0 0 0 0\n"
      <<"1 3 Car 0 0 0 12 20 32 40 0 0 0 0 0 0 0\n"
      <<"1 7 Car 1 2 0 52 20 72 40 0 0 0 0 0 0 0\n";}
    auto original=tracking::read_kitti(path);
    {std::ofstream o(path);o<<"1 93 Car 1 2 0 52 20 72 40 0 0 0 0 0 0 0\n"
      <<"0 23 Car 2 3 0 50 20 70 40 0 0 0 0 0 0 0\n"
      <<"0 93 Car 0 0 0 10 20 30 40 0 0 0 0 0 0 0\n"
      <<"1 23 Car 0 0 0 12 20 32 40 0 0 0 0 0 0 0\n";}
    auto relabeled=tracking::read_kitti(path);std::remove(path.c_str());
    auto boxes=[](const tracking::Frame&f){std::vector<Box>v;for(auto d:f.detections)v.push_back(d.box);return v;};
    for(const std::string metric:{"distance","iou"})for(const std::string method:{"hungarian","greedy"}){
        auto a=tracking::associate(boxes(original[0]),boxes(original[1]),metric,method);
        auto b=tracking::associate(boxes(relabeled[0]),boxes(relabeled[1]),metric,method);
        check(a.detection_of_track==b.detection_of_track);check(a.objective==b.objective);
    }
    check(original[0].detections[1].truncation==2);check(original[0].detections[1].occlusion==3);
    for(auto penalty:{0LL,400000LL,600000LL,800000LL,1000000LL})for(double gate:{40.0,80.0,160.0})for(double overlap:{0.0,0.05,0.2,1.0})for(const std::string metric:{"distance","iou"}){
        auto a=tracking::augmented_cost(t,d,metric,penalty,gate,overlap);
        auto h=tracking::associate(t,d,metric,"hungarian",penalty,gate,overlap);
        check(oracle(a,h.objective));check(h.objective<=tracking::associate(t,d,metric,"greedy",penalty,gate,overlap).objective);
    }
    auto rejects=[&](auto fn){bool threw=false;try{fn();}catch(const std::exception&){threw=true;}check(threw);};
    rejects([&](){tracking::augmented_cost(t,d,"distance",-1);});
    rejects([&](){tracking::augmented_cost(t,d,"distance",1000001);});
    rejects([&](){tracking::augmented_cost(t,d,"distance",600000,0);});
    rejects([&](){tracking::augmented_cost(t,d,"iou",600000,80,1.1);});
    rejects([&](){tracking::augmented_cost(t,d,"unknown");});
    rejects([&](){tracking::associate(t,d,"iou","unknown");});
    rejects([&](){tracking::augmented_cost({{0,0,-1,1}},d,"iou");});
    for(const std::string row:{"0 1 Car 0 0 0 1 2 3 4\n", "0 1 Car 0 0 0 3 2 1 4 0 0 0 0 0 0 0\n", "0 1 Car 0 0 0 1 2 3 4 0 0 0 0 0 0 0 extra\n", "0 1 Car 0 0 0 1 2 3 4 0 0 0 0 0 0 0\n0 1 Car 0 0 0 5 2 7 4 0 0 0 0 0 0 0\n", "0 1 Car 0 0 0 1 2 3 4 0 0 0 0 0 0 0\n0 2 Car 0 0 0 1 2 3 4 0 0 0 0 0 0 0\n"}){
        {std::ofstream o(path);o<<row;}rejects([&](){tracking::read_kitti(path);});std::remove(path.c_str());
    }
    std::cout<<"PASS: "<<checks<<" tracking checks; "<<instances<<" optional oracle instances.\n";return 0;
}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
