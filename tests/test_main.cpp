#include "archpulse/cache.hpp"
#include "archpulse/pipeline.hpp"
#include "archpulse/thread_pool.hpp"
#include <cassert>
#include <iostream>
#include <vector>
int main(){archpulse::CacheConfig cfg;cfg.cache_size_bytes=128;cfg.block_size_bytes=64;cfg.ways=2;archpulse::CacheSimulator cache(cfg);assert(!cache.access(0));assert(!cache.access(64));assert(cache.access(0));assert(!cache.access(128));assert(cache.access(0));assert(cache.stats().hits==2&&cache.stats().misses==3&&cache.stats().evictions==1);archpulse::BranchPredictor p(archpulse::PredictorKind::TwoBit);assert(p.predict());p.update(false);assert(!p.predict());p.update(false);assert(!p.predict());p.update(true);assert(!p.predict());p.update(true);assert(p.predict());std::vector<archpulse::Instruction> prog={{archpulse::OpCode::LOAD,1,0,-1,false},{archpulse::OpCode::ADD,2,1,3,false},{archpulse::OpCode::BRANCH,-1,2,-1,false},{archpulse::OpCode::MUL,4,2,5,false}};archpulse::PipelineSimulator sim(archpulse::PredictorKind::AlwaysTaken);auto s=sim.run(prog);assert(s.instructions==4&&s.data_hazards>=2&&s.branches==1&&s.mispredictions==1);archpulse::ThreadPool pool(2);auto a=pool.submit([]{return 20;});auto b=pool.submit([](int x){return x*2;},11);assert(a.get()==20&&b.get()==22);std::cout<<"All ArchPulse tests passed.\n";}
