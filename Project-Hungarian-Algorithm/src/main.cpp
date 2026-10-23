#include "assignment.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc,char**argv){try{
    lap::Matrix a;
    if(argc==2){std::ifstream in(argv[1]);std::size_t r,c;
        if(!(in>>r>>c))throw std::runtime_error("Expected rows cols followed by integer costs");
        a=lap::Matrix(r,c);for(auto&x:a.values)if(!(in>>x))throw std::runtime_error("Missing cost");
    }else{a=lap::Matrix(3,3);a.values={4,1,3,2,0,5,3,2,2};}
    auto h=lap::hungarian(a,true),g=lap::greedy(a);
    std::cout<<"Hungarian total = "<<h.cost<<"; Greedy total = "<<g.cost<<'\n';
    for(std::size_t i=0;i<a.rows;++i)std::cout<<"row "<<i<<" -> column "<<h.column_of_row[i]<<'\n';
    std::cout<<"dual u:";for(auto x:h.u)std::cout<<' '<<x;
    std::cout<<"\ndual v:";for(auto x:h.v)std::cout<<' '<<x;
    std::cout<<"\ncertificate = "<<(lap::valid_certificate(a,h)?"PASS":"FAIL")<<'\n';
    return lap::valid_certificate(a,h)?0:1;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
