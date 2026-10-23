#include "assignment.hpp"
#include <algorithm>
#include <functional>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <tuple>

namespace lap {
namespace {
constexpr Cost inf = std::numeric_limits<Cost>::max() / 4;
void validate(const Matrix& a) {
    if (a.rows > max_dimension || a.cols > max_dimension ||
        a.values.size() != a.rows * a.cols)
        throw std::invalid_argument("Invalid matrix dimensions or storage");
    for (Cost c : a.values)
        if (c < -max_abs_cost || c > max_abs_cost)
            throw std::out_of_range("Costs must lie in [-1e9, 1e9]");
}
Matrix transpose(const Matrix& a) {
    Matrix b(a.cols, a.rows);
    for (std::size_t i=0;i<a.rows;++i)
        for (std::size_t j=0;j<a.cols;++j) b(j,i)=a(i,j);
    return b;
}
template<bool Count>
Result solve_wide(const Matrix& a) {
    const int r=static_cast<int>(a.rows), c=static_cast<int>(a.cols);
    Result out;
    out.column_of_row.assign(r,-1);
    out.u.assign(r,0); out.v.assign(c,0);
    if(r==0) return out;
    std::vector<Cost> u(r+1,0),v(c+1,0),slack(c+1,inf);
    std::vector<int> p(c+1,0),way(c+1,0);
    std::vector<unsigned char> used(c+1,0);
    for(int i=1;i<=r;++i) {
        u[i]=a(i-1,0);
        for(int j=1;j<=c;++j) u[i]=std::min(u[i],a(i-1,j-1));
    }
    // Initially all reduced costs are nonnegative, including for negative input.
    for(int i=1;i<=r;++i) {
        p[0]=i; int j0=0;
        std::fill(slack.begin(),slack.end(),inf);
        std::fill(used.begin(),used.end(),0);
        do {
            used[j0]=1;
            const int i0=p[j0];
            Cost delta=inf; int j1=-1;
            if constexpr(Count) ++out.operations.expansions;
            for(int j=1;j<=c;++j) if(!used[j]) {
                if constexpr(Count) ++out.operations.scans;
                const Cost reduced=a(i0-1,j-1)-u[i0]-v[j];
                if(reduced<slack[j]) {slack[j]=reduced;way[j]=j0;}
                // Strict comparison and increasing j give deterministic ties.
                if(slack[j]<delta) {delta=slack[j];j1=j;}
            }
            if(j1<0 || delta==inf || delta<0)
                throw std::logic_error("Broken feasible-potential invariant");
            if constexpr(Count) if(delta>0) ++out.operations.positive_updates;
            for(int j=0;j<=c;++j) {
                if(used[j]) {u[p[j]]+=delta;if(j!=0) v[j]-=delta;}
                else if(j!=0) slack[j]-=delta;
            }
            j0=j1;
        } while(p[j0]!=0);
        do {const int prev=way[j0];p[j0]=p[prev];j0=prev;} while(j0!=0);
    }
    for(int j=1;j<=c;++j) if(p[j]) {
        out.column_of_row[p[j]-1]=j-1;
        out.cost+=a(p[j]-1,j-1);
    }
    std::copy(u.begin()+1,u.end(),out.u.begin());
    std::copy(v.begin()+1,v.end(),out.v.begin());
    out.workspace_bytes=(u.capacity()+v.capacity()+slack.capacity()+
        out.u.capacity()+out.v.capacity())*sizeof(Cost)+
        (p.capacity()+way.capacity()+out.column_of_row.capacity())*sizeof(int)+
        used.capacity()*sizeof(unsigned char);
    return out;
}
}

Matrix::Matrix(std::size_t r,std::size_t c):rows(r),cols(c) {
    if(r>max_dimension||c>max_dimension) throw std::invalid_argument("Dimension > 10000");
    values.assign(r*c,0);
}
Cost& Matrix::operator()(std::size_t i,std::size_t j){return values[i*cols+j];}
Cost Matrix::operator()(std::size_t i,std::size_t j)const{return values[i*cols+j];}
Result hungarian(const Matrix& input,bool counters) {
    validate(input);
    if(input.rows<=input.cols) return counters?solve_wide<true>(input):solve_wide<false>(input);
    Matrix a=transpose(input);
    Result out=counters?solve_wide<true>(a):solve_wide<false>(a);
    const std::size_t solving_peak=out.workspace_bytes+a.values.capacity()*sizeof(Cost);
    std::vector<int> restored(input.rows,-1);
    for(std::size_t j=0;j<out.column_of_row.size();++j)
        restored[out.column_of_row[j]]=static_cast<int>(j);
    const std::size_t mapping_peak=(a.values.capacity()+out.u.capacity()+out.v.capacity())*sizeof(Cost)+
        (out.column_of_row.capacity()+restored.capacity())*sizeof(int);
    out.workspace_bytes=std::max(solving_peak,mapping_peak);
    out.column_of_row.swap(restored);out.transposed=true;
    return out;
}
Result greedy(const Matrix& input) {
    validate(input);
    struct Edge{Cost cost;int row,col;};
    std::vector<Edge> edges;edges.reserve(input.rows*input.cols);
    for(std::size_t i=0;i<input.rows;++i)
        for(std::size_t j=0;j<input.cols;++j)
            edges.push_back({input(i,j),static_cast<int>(i),static_cast<int>(j)});
    std::sort(edges.begin(),edges.end(),[](const Edge&a,const Edge&b){
        return std::tie(a.cost,a.row,a.col)<std::tie(b.cost,b.row,b.col);});
    Result out;out.column_of_row.assign(input.rows,-1);
    std::vector<unsigned char> taken(input.cols,0);
    std::size_t count=0;
    for(const auto&e:edges) if(out.column_of_row[e.row]<0&&!taken[e.col]) {
        out.column_of_row[e.row]=e.col;taken[e.col]=1;out.cost+=e.cost;
        if(++count==std::min(input.rows,input.cols)) break;
    }
    out.workspace_bytes=edges.capacity()*sizeof(Edge)+taken.capacity()+
        out.column_of_row.capacity()*sizeof(int);
    return out;
}
Result exhaustive(const Matrix& input) {
    validate(input);
    if(std::max(input.rows,input.cols)>10) throw std::invalid_argument("Exhaustive dimension > 10");
    bool flip=input.rows>input.cols;
    Matrix a=flip?transpose(input):input;
    Result out;out.cost=inf;out.column_of_row.assign(a.rows,-1);
    std::vector<int> cur(a.rows,-1);
    std::vector<unsigned char> used(a.cols,0);
    std::function<void(std::size_t,Cost)> visit=[&](std::size_t i,Cost total){
        if(i==a.rows){if(total<out.cost){out.cost=total;out.column_of_row=cur;}return;}
        for(std::size_t j=0;j<a.cols;++j) if(!used[j]) {
            used[j]=1;cur[i]=static_cast<int>(j);
            visit(i+1,total+a(i,j));used[j]=0;
        }
    };
    visit(0,0);
    if(flip){std::vector<int> restored(input.rows,-1);
        for(std::size_t j=0;j<out.column_of_row.size();++j)
            restored[out.column_of_row[j]]=static_cast<int>(j);
        out.column_of_row.swap(restored);out.transposed=true;}
    return out;
}
bool valid_assignment(const Matrix&a,const Result&r) {
    if(r.column_of_row.size()!=a.rows) return false;
    std::vector<bool> used(a.cols,false);Cost cost=0;std::size_t count=0;
    for(std::size_t i=0;i<a.rows;++i){int j=r.column_of_row[i];
        if(j<0){if(j!=-1)return false;continue;}
        if(static_cast<std::size_t>(j)>=a.cols||used[j])return false;
        used[j]=true;++count;cost+=a(i,j);}
    return count==std::min(a.rows,a.cols)&&cost==r.cost;
}
bool valid_certificate(const Matrix&input,const Result&r) {
    if(!valid_assignment(input,r)||r.transposed!=(input.rows>input.cols))return false;
    const auto nr=std::min(input.rows,input.cols),nc=std::max(input.rows,input.cols);
    if(r.u.size()!=nr||r.v.size()!=nc)return false;
    Cost dual=0;for(auto u:r.u)dual+=u;
    for(auto v:r.v){if(v>0)return false;dual+=v;}
    for(std::size_t i=0;i<nr;++i)for(std::size_t j=0;j<nc;++j)
        if(r.u[i]+r.v[j]>(r.transposed?input(j,i):input(i,j)))return false;
    std::vector<bool> used(nc,false);
    for(std::size_t i=0;i<input.rows;++i){int j=r.column_of_row[i];if(j<0)continue;
        const auto row=r.transposed?static_cast<std::size_t>(j):i;
        const auto col=r.transposed?i:static_cast<std::size_t>(j);
        if(r.u[row]+r.v[col]!=input(i,j))return false;
        used[col]=true;
    }
    for(std::size_t j=0;j<nc;++j)if(!used[j]&&r.v[j]!=0)return false;
    return dual==r.cost;
}
Matrix generate(std::size_t r,std::size_t c,const std::string&family,std::uint64_t seed){
    Matrix a(r,c);std::mt19937_64 rng(seed);
    // Modulo mapping specifies a portable sequence. Its bias is below 1e-13 here.
    for(std::size_t i=0;i<r;++i)for(std::size_t j=0;j<c;++j){
        if(family=="uniform")a(i,j)=1+static_cast<Cost>(rng()%1000000ULL);
        else if(family=="planted")a(i,j)=i==j?1+static_cast<Cost>(rng()%100ULL):1000000+static_cast<Cost>(rng()%10000ULL);
        else if(family=="near_ties")a(i,j)=100+static_cast<Cost>(rng()%4ULL);
        else if(family=="equal")a(i,j)=100;
        else if(family=="adversarial"){
            if(i/2!=j/2)a(i,j)=1000000;
            else if(i%2==0&&j%2==0)a(i,j)=1;
            else if(i%2==1&&j%2==1)a(i,j)=1000;
            else a(i,j)=2;
        } else throw std::invalid_argument("Unknown family");
    }return a;
}
std::uint64_t fingerprint(const Matrix&a){
    std::uint64_t h=14695981039346656037ULL;
    auto mix=[&](std::uint64_t x){for(int k=0;k<8;++k){h^=(x>>(8*k))&255ULL;h*=1099511628211ULL;}};
    mix(a.rows);mix(a.cols);for(auto x:a.values)mix(static_cast<std::uint64_t>(x));return h;
}
}
