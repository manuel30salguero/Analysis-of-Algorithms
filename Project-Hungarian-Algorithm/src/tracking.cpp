#include "tracking.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <set>

namespace tracking {
namespace {
constexpr lap::Cost scale=1000000;
void check_box(const Box&b){
    if(!std::isfinite(b.cx)||!std::isfinite(b.cy)||!std::isfinite(b.width)||
       !std::isfinite(b.height)||b.width<=0||b.height<=0)
        throw std::invalid_argument("Invalid bounding box");
}
lap::Cost cost(const Box&a,const Box&b,const std::string&metric,double gate,double iou_gate,lap::Cost forbidden){
    if(metric=="distance"){
        const double d=std::hypot(a.cx-b.cx,a.cy-b.cy);
        return d>gate?forbidden:static_cast<lap::Cost>(std::llround(scale*d/gate));
    }
    if(metric=="iou"){
        double overlap=iou(a,b);
        return overlap<iou_gate?forbidden:static_cast<lap::Cost>(std::llround(scale*(1-overlap)));
    }
    throw std::invalid_argument("Metric must be distance or iou");
}
}
double iou(const Box&a,const Box&b){
    check_box(a);check_box(b);
    double w=std::max(0.0,std::min(a.cx+a.width/2,b.cx+b.width/2)-std::max(a.cx-a.width/2,b.cx-b.width/2));
    double h=std::max(0.0,std::min(a.cy+a.height/2,b.cy+b.height/2)-std::max(a.cy-a.height/2,b.cy-b.height/2));
    double intersection=w*h;return intersection/(a.width*a.height+b.width*b.height-intersection);
}
lap::Matrix augmented_cost(const std::vector<Box>&tracks,const std::vector<Box>&detections,
    const std::string&metric,lap::Cost penalty,double gate,double iou_gate){
    if(penalty<0||penalty>1000000||!std::isfinite(gate)||gate<=0||!std::isfinite(iou_gate)||iou_gate<0||iou_gate>1)
        throw std::invalid_argument("Invalid penalty or gate");
    if(metric!="distance"&&metric!="iou")throw std::invalid_argument("Unknown metric");
    for(const auto&b:tracks)check_box(b);
    for(const auto&b:detections)check_box(b);
    const std::size_t t=tracks.size(),d=detections.size(),q=t+d;
    const lap::Cost forbidden=static_cast<lap::Cost>(q)*penalty+scale+1;
    if(forbidden>lap::max_abs_cost)throw std::invalid_argument("Augmented cost exceeds numeric contract");
    lap::Matrix a(q,q);std::fill(a.values.begin(),a.values.end(),forbidden);
    for(std::size_t i=0;i<t;++i){
        for(std::size_t j=0;j<d;++j)a(i,j)=cost(tracks[i],detections[j],metric,gate,iou_gate,forbidden);
        a(i,d+i)=penalty;
    }
    for(std::size_t j=0;j<d;++j){a(t+j,j)=penalty;
        for(std::size_t i=0;i<t;++i)a(t+j,d+i)=0;}
    return a;
}
Association associate(const std::vector<Box>&tracks,const std::vector<Box>&detections,
    const std::string&metric,const std::string&method,lap::Cost penalty,double gate,double iou_gate){
    auto a=augmented_cost(tracks,detections,metric,penalty,gate,iou_gate);
    const auto t=tracks.size(),d=detections.size();
    Association out;out.detection_of_track.assign(t,-1);
    if(method=="hungarian"){
        auto h=lap::hungarian(a);out.objective=h.cost;
        for(std::size_t i=0;i<t;++i)if(h.column_of_row[i]>=0&&static_cast<std::size_t>(h.column_of_row[i])<d)
            out.detection_of_track[i]=h.column_of_row[i];
    }else if(method=="greedy"){
        struct Edge{lap::Cost value;int i,j;};std::vector<Edge>edges;
        for(std::size_t i=0;i<t;++i)for(std::size_t j=0;j<d;++j)
            if(a(i,j)<2*penalty)edges.push_back({a(i,j),static_cast<int>(i),static_cast<int>(j)});
        std::sort(edges.begin(),edges.end(),[](const Edge&a,const Edge&b){
            return std::tie(a.value,a.i,a.j)<std::tie(b.value,b.i,b.j);});
        std::vector<bool>used(d,false);out.objective=static_cast<lap::Cost>(t+d)*penalty;
        for(const auto&e:edges)if(out.detection_of_track[e.i]<0&&!used[e.j]){
            out.detection_of_track[e.i]=e.j;used[e.j]=true;out.objective+=e.value-2*penalty;}
    }else throw std::invalid_argument("Unknown association method");
    return out;
}
std::vector<Frame> synthetic_sequence(int population,int frame_count,std::uint64_t seed,const std::string&scenario){
    if(population<=0||frame_count<=0||(scenario!="clean"&&scenario!="ambiguous"))throw std::invalid_argument("Invalid synthetic setup");
    std::mt19937_64 rng(seed);auto unit=[&](){return static_cast<double>(rng()%1000001ULL)/1000000.0;};
    const double noise=scenario=="clean"?0.5:6.0,miss=scenario=="clean"?0.02:0.15;
    std::vector<Frame> frames;
    for(int f=0;f<frame_count;++f){Frame frame{f,{}};
        for(int id=0;id<population;++id){
            int birth=(id%5)*5,death=95+(id*7)%45;
            if(f<birth||f>=death)continue;
            if(scenario=="ambiguous"&&id%4==0&&f>=50&&f<55)continue;
            if(unit()<miss)continue;
            int lane=id%4,group=id/4;
            double velocity=group%2==0?2.5:-2.5;
            double x=100+group*(800.0/(population/4+1))+velocity*(f-birth);
            double y=100+34*lane+2*std::sin(0.06*f+id);
            frame.detections.push_back({{x+noise*(2*unit()-1),y+noise*(2*unit()-1),28,16},id});
        }
        if(scenario=="ambiguous")for(int k=0;k<population/16;++k)
            frame.detections.push_back({{100+800*unit(),90+140*unit(),28,16},-1});
        // Fisher-Yates permutation removes truth-ID ordering information.
        for(std::size_t k=frame.detections.size();k>1;--k)
            std::swap(frame.detections[k-1],frame.detections[rng()%k]);
        frames.push_back(std::move(frame));
    }return frames;
}
std::vector<Frame> read_kitti(const std::string&path){
    std::ifstream in(path);if(!in)throw std::runtime_error("Cannot read local KITTI labels");
    std::map<int,Frame> frames;std::string line;int maximum=-1;
    while(std::getline(in,line)){
        if(line.empty())continue;
        std::istringstream s(line);int f,id,occluded,truncated;std::string type,extra;
        double alpha,l,t,r,b,h,w,length,x,y,z,rotation;
        if(!(s>>f>>id>>type>>truncated>>occluded>>alpha>>l>>t>>r>>b>>h>>w>>length>>x>>y>>z>>rotation)||(s>>extra))
            throw std::runtime_error("Expected exactly 17 KITTI training label fields");
        if(f<0)throw std::runtime_error("Negative KITTI frame index");
        maximum=std::max(maximum,f);
        if(type!="Car")continue;
        // Reject malformed selected records rather than silently changing the sample.
        if(id<0||truncated<0||truncated>2||occluded<0||occluded>3)
            throw std::runtime_error("Invalid Car annotation metadata");
        Box box{(l+r)/2,(t+b)/2,r-l,b-t};check_box(box);
        frames[f].detections.push_back({box,id,truncated,occluded});
    }
    std::vector<Frame>out;
    for(int f=0;f<=maximum;++f){
        auto& frame=frames[f];frame.index=f;
        // Geometry is the ONLY ordering key. This removes annotation/identity order.
        auto key=[](const Detection& d){return std::tie(d.box.cx,d.box.cy,d.box.width,d.box.height);};
        std::sort(frame.detections.begin(),frame.detections.end(),[&](const Detection&a,const Detection&b){return key(a)<key(b);});
        std::set<int> ids;
        for(std::size_t k=0;k<frame.detections.size();++k){
            if(!ids.insert(frame.detections[k].truth_id).second)throw std::runtime_error("Duplicate Car identity in one frame");
            if(k&&key(frame.detections[k-1])==key(frame.detections[k]))throw std::runtime_error("Duplicate Car geometry: deterministic ordering undefined");
        }
        out.push_back(std::move(frame));
    }
    return out;
}
}
