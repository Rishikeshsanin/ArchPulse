#include "archpulse/benchmark.hpp"
#include "archpulse/thread_pool.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>
namespace archpulse { namespace { using Clock=std::chrono::steady_clock; std::vector<double> matrix(std::size_t n,std::uint32_t seed){std::mt19937 rng(seed);std::uniform_real_distribution<double>d(-1,1);std::vector<double>m(n*n);for(auto&v:m)v=d(rng);return m;} double checksum(const std::vector<double>&v){return std::accumulate(v.begin(),v.end(),0.0);} }
BenchmarkResult benchmark_matrix_multiplication(std::size_t n,std::size_t threads,double baseline){if(!n||!threads)throw std::invalid_argument("matrix size and threads must be non-zero");auto a=matrix(n,7),b=matrix(n,11);std::vector<double>c(n*n,0);auto start=Clock::now();{ThreadPool pool(threads);const std::size_t chunk=(n+threads-1)/threads;std::vector<std::future<void>>fs;for(std::size_t begin=0;begin<n;begin+=chunk){auto end=std::min(n,begin+chunk);fs.push_back(pool.submit([&,begin,end](){for(std::size_t i=begin;i<end;++i)for(std::size_t k=0;k<n;++k){double aik=a[i*n+k];for(std::size_t j=0;j<n;++j)c[i*n+j]+=aik*b[k*n+j];}}));}for(auto&f:fs)f.get();}double ms=std::chrono::duration<double,std::milli>(Clock::now()-start).count();double ops=2.0*n*n*n;return{"matrix_multiplication",n,threads,ms,ms?ops/(ms/1000.0)/1e9:0,baseline?baseline/ms:1,std::isfinite(checksum(c))};}
BenchmarkResult benchmark_convolution(std::size_t n,std::size_t threads,double baseline){if(n<3||!threads)throw std::invalid_argument("image size must be >=3 and threads non-zero");std::vector<double>in(n*n),out(n*n);for(std::size_t i=0;i<in.size();++i)in[i]=static_cast<double>((i*17)%255)/255.0;constexpr double k[3][3]={{1./16,2./16,1./16},{2./16,4./16,2./16},{1./16,2./16,1./16}};auto start=Clock::now();{ThreadPool pool(threads);std::size_t interior=n-2,chunk=(interior+threads-1)/threads;std::vector<std::future<void>>fs;for(std::size_t off=0;off<interior;off+=chunk){std::size_t rb=1+off,re=std::min(n-1,rb+chunk);fs.push_back(pool.submit([&,rb,re](){for(std::size_t r=rb;r<re;++r)for(std::size_t c=1;c+1<n;++c){double v=0;for(int kr=-1;kr<=1;++kr)for(int kc=-1;kc<=1;++kc)v+=in[(r+kr)*n+(c+kc)]*k[kr+1][kc+1];out[r*n+c]=v;}}));}for(auto&f:fs)f.get();}double ms=std::chrono::duration<double,std::milli>(Clock::now()-start).count();double pixels=static_cast<double>((n-2)*(n-2));return{"gaussian_convolution",n,threads,ms,ms?pixels/(ms/1000.0)/1e6:0,baseline?baseline/ms:1,std::isfinite(checksum(out))};}
std::vector<BenchmarkResult> run_scaling_suite(std::size_t size,const std::vector<std::size_t>& counts){std::vector<BenchmarkResult>out;double baseline=0;for(auto t:counts){auto r=benchmark_matrix_multiplication(size,t,baseline);if(!baseline){baseline=r.runtime_ms;r.speedup=1;}out.push_back(r);}return out;}
}
