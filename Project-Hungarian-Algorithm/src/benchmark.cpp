#include "assignment.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/resource.h>

namespace {
using Clock=std::chrono::steady_clock;
volatile lap::Cost sink=0;
lap::Result solve(const lap::Matrix&a,const std::string&name){
    if(name=="hungarian")return lap::hungarian(a);
    if(name=="greedy")return lap::greedy(a);
    if(name=="exhaustive")return lap::exhaustive(a);
    throw std::invalid_argument("Unknown algorithm");
}
double batch(const lap::Matrix&a,const std::string&name,int repeat){
    auto start=Clock::now();
    for(int k=0;k<repeat;++k){const auto r=solve(a,name);sink=r.cost;}
    return std::chrono::duration<double,std::micro>(Clock::now()-start).count()/repeat;
}
void record(std::ostream&out,const lap::Matrix&a,const std::string&family,
            std::uint64_t seed,const std::string&name,int repetitions){
    auto h=lap::hungarian(a,true),r=solve(a,name);
    if(!lap::valid_certificate(a,h)||!lap::valid_assignment(a,r)||r.cost<h.cost)
        throw std::runtime_error("Benchmark validation failure");
    if(name=="exhaustive"&&r.cost!=h.cost)throw std::runtime_error("Oracle mismatch");
    int inner=1;double pilot=batch(a,name,1);
    if(pilot>0)inner=std::max(1,std::min(256,static_cast<int>(2000.0/pilot)));
    for(int rep=0;rep<repetitions;++rep){
        double us=batch(a,name,inner);
        out<<family<<','<<a.rows<<','<<a.cols<<','<<seed<<','<<name<<','<<rep<<','<<inner<<','
           <<std::setprecision(12)<<us<<','<<r.cost<<','<<h.cost<<','<<(r.cost-h.cost)<<','
           <<(name=="hungarian"?h.operations.scans:0)<<','
           <<(name=="hungarian"?h.operations.expansions:0)<<','<<r.workspace_bytes<<','
           <<lap::fingerprint(a)<<'\n';
    }
}
}
int main(int argc,char**argv){try{
    if(argc>=5&&std::string(argv[1])=="memory"){
        const auto n=static_cast<std::size_t>(std::stoul(argv[3]));
        auto a=lap::generate(n,n,argv[4],20261021);auto r=solve(a,argv[2]);
        rusage usage{};getrusage(RUSAGE_SELF,&usage);
        std::cout<<argv[2]<<','<<n<<','<<a.values.capacity()*sizeof(lap::Cost)<<','
          <<r.workspace_bytes<<','<<usage.ru_maxrss<<','<<r.cost<<'\n';return 0;
    }
    if(argc!=3)throw std::invalid_argument("Usage: lap_benchmark full|small|rect output.csv; or memory algorithm n family");
    std::ofstream out(argv[2]);if(!out)throw std::runtime_error("Cannot write results");
    out<<"family,rows,cols,seed,algorithm,rep,inner,time_us,cost,optimum,absolute_gap,scans,expansions,workspace_bytes,fingerprint\n";
    std::string mode=argv[1];
    if(mode=="full")for(const std::string f:{"uniform","planted","near_ties","equal","adversarial"}){
        for(int n:{16,32,64,128,256,512,1024}){
            // Deterministic families need timing repetitions, not duplicated instances.
            int seeds=(f=="equal"||f=="adversarial")?1:7;
            for(int s=0;s<seeds;++s){std::uint64_t seed=20261021+1009*s;
                auto a=lap::generate(n,n,f,seed);
                // Alternate method order to reduce systematic order effects.
                if(s%2==0){record(out,a,f,seed,"hungarian",3);record(out,a,f,seed,"greedy",3);}
                else{record(out,a,f,seed,"greedy",3);record(out,a,f,seed,"hungarian",3);}
            }std::cerr<<f<<" n="<<n<<" complete\n";
        }
    }else if(mode=="small")for(int n=2;n<=10;++n)for(int s=0;s<5;++s){
        std::uint64_t seed=20261021+1009*s;auto a=lap::generate(n,n,"uniform",seed);
        for(const std::string name:{"hungarian","greedy","exhaustive"})record(out,a,"uniform",seed,name,3);
    }else if(mode=="rect")for(auto dims:{std::pair<int,int>{32,128},{64,256},{128,512},{256,1024},{512,128}})
        for(int s=0;s<7;++s){std::uint64_t seed=20261021+1009*s;
            auto a=lap::generate(dims.first,dims.second,"uniform",seed);
            for(const std::string name:{"hungarian","greedy"})record(out,a,"uniform",seed,name,3);
        }
    else throw std::invalid_argument("Unknown benchmark mode");
    return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
