#include "assignment.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>

static std::size_t checks=0,instances=0;
static std::ofstream evidence;
static void check(bool condition,const std::string& message){
    ++checks;if(!condition)throw std::runtime_error(message);
}
static void compare(const lap::Matrix&a){
    ++instances;
    const auto h=lap::hungarian(a,true), b=lap::exhaustive(a), g=lap::greedy(a);
    check(lap::valid_certificate(a,h),"Invalid primal-dual certificate");
    check(lap::valid_assignment(a,b),"Invalid exhaustive assignment");
    check(lap::valid_assignment(a,g),"Invalid greedy assignment");
    check(h.cost==b.cost,"Hungarian differs from exhaustive search");
    check(g.cost>=h.cost,"Greedy lower than certified optimum");
    check(lap::hungarian(a).column_of_row==h.column_of_row,"Nondeterministic assignment");
    if(evidence)evidence<<instances<<','<<a.rows<<','<<a.cols<<','<<lap::fingerprint(a)<<','<<h.cost<<','<<b.cost<<','<<g.cost<<",1\n";
}
int main(int argc,char**argv){try{
    if(argc>1){evidence.open(argv[1]);if(!evidence)throw std::runtime_error("Cannot write oracle CSV");evidence<<"instance,rows,cols,fingerprint,hungarian_cost,exhaustive_cost,greedy_cost,certificate_ok\n";}
    lap::Matrix known(3,3);known.values={4,1,3,2,0,5,3,2,2};compare(known);
    check(lap::hungarian(known).cost==5,"Known optimum is 5");
    for(auto dims:{std::pair<int,int>{0,0},{0,4},{4,0},{1,1},{2,5},{5,2}})
        compare(lap::Matrix(dims.first,dims.second));
    for(const std::string f:{"uniform","planted","near_ties","equal","adversarial"})
        for(int n=1;n<=8;++n)for(int s=0;s<12;++s)compare(lap::generate(n,n,f,20261021+s));
    std::mt19937_64 rng(67477245);
    for(int r=1;r<=5;++r)for(int c=1;c<=5;++c)for(int s=0;s<40;++s){
        lap::Matrix a(r,c);for(auto&x:a.values)x=static_cast<lap::Cost>(rng()%31ULL)-15;
        compare(a);
        auto shifted=a;for(auto&x:shifted.values)x+=37;
        const auto h=lap::hungarian(a),hs=lap::hungarian(shifted);
        check(hs.cost==h.cost+37*std::min(r,c),"Constant shift invariance");
        // Row shifts preserve a full assignment only when every row is selected.
        if(r<=c){for(int i=0;i<r;++i)for(int j=0;j<c;++j)shifted(i,j)=a(i,j)+i;
            check(lap::hungarian(shifted).cost==h.cost+r*(r-1)/2,"Row shift invariance");}
    }
    // Exhaust every 2x3 cost matrix over {-1,0,1}, with its transpose.
    for(int mask=0;mask<729;++mask){int x=mask;lap::Matrix a(2,3),t(3,2);
        for(int i=0;i<2;++i)for(int j=0;j<3;++j){a(i,j)=x%3-1;x/=3;t(j,i)=a(i,j);}
        compare(a);compare(t);}
    lap::Matrix extremes(3,3);extremes.values={-1000000000,1000000000,0,1000000000,-1000000000,0,0,0,-1000000000};compare(extremes);
    check(lap::hungarian(extremes).cost==-3000000000LL,"64-bit total");
    auto bad=known;bad.values[0]=1000000001;
    bool threw=false;try{lap::hungarian(bad);}catch(const std::out_of_range&){threw=true;}check(threw,"Reject unsafe cost");
    bad=known;bad.values.pop_back();threw=false;
    try{lap::hungarian(bad);}catch(const std::invalid_argument&){threw=true;}check(threw,"Reject malformed storage");
    auto certificate=lap::hungarian(known);++certificate.u[0];
    check(!lap::valid_certificate(known,certificate),"Reject corrupted certificate");
    std::cout<<"PASS: "<<instances<<" oracle-compared instances; "<<checks<<" checks.\n";
    return 0;
}catch(const std::exception&e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}}
