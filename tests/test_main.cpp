#include "archpulse/cache.hpp"
#include "archpulse/coherence.hpp"
#include "archpulse/memory_hierarchy.hpp"
#include "archpulse/pipeline.hpp"
#include "archpulse/thread_pool.hpp"
#include "archpulse/tlb.hpp"
#include <cassert>
#include <iostream>
#include <vector>
int main(){
  archpulse::CacheConfig cfg;cfg.cache_size_bytes=128;cfg.block_size_bytes=64;cfg.ways=2;archpulse::CacheSimulator cache(cfg);assert(!cache.access(0));assert(!cache.access(64));assert(cache.access(0));assert(!cache.access(128));assert(cache.access(0));assert(cache.stats().hits==2&&cache.stats().misses==3&&cache.stats().evictions==1);
  archpulse::TlbConfig tc;tc.entries=4;tc.ways=2;tc.page_size_bytes=4096;archpulse::TlbSimulator tlb(tc);assert(!tlb.access(0));assert(tlb.access(0));assert(!tlb.access(4096));assert(tlb.stats().hits==1&&tlb.stats().misses==2);
  archpulse::MemoryHierarchyConfig mc;mc.l1.cache_size_bytes=128;mc.l1.block_size_bytes=64;mc.l1.ways=2;mc.l2.cache_size_bytes=256;mc.l2.block_size_bytes=64;mc.l2.ways=2;mc.tlb=tc;archpulse::MemoryHierarchySimulator hierarchy(mc);hierarchy.access(0);hierarchy.access(0);const auto& hs=hierarchy.stats();assert(hs.accesses==2&&hs.l1_hits==1&&hs.l1_misses==1&&hs.tlb_hits==1&&hs.dram_accesses==1);
  archpulse::MesiSimulator mesi(4,64);assert(!mesi.read(0,0));assert(mesi.state(0,0)==archpulse::MesiState::Exclusive);assert(!mesi.read(1,0));assert(mesi.state(0,0)==archpulse::MesiState::Shared&&mesi.state(1,0)==archpulse::MesiState::Shared);assert(mesi.write(1,0));assert(mesi.state(0,0)==archpulse::MesiState::Invalid&&mesi.state(1,0)==archpulse::MesiState::Modified);assert(!mesi.read(0,0));assert(mesi.state(0,0)==archpulse::MesiState::Shared&&mesi.state(1,0)==archpulse::MesiState::Shared);assert(mesi.stats().invalidations>=1&&mesi.stats().writebacks>=1&&mesi.stats().bus_reads==3);
  archpulse::BranchPredictor p(archpulse::PredictorKind::TwoBit);assert(p.predict());p.update(false);assert(!p.predict());p.update(false);assert(!p.predict());p.update(true);assert(!p.predict());p.update(true);assert(p.predict());
  archpulse::BranchPredictor g(archpulse::PredictorKind::GShare);for(std::uint64_t i=0;i<32;++i){const bool taken=(i%4)!=0;(void)g.predict(i);g.update(taken,i);}g.reset();assert(g.predict(7));
  std::vector<archpulse::Instruction> prog={{archpulse::OpCode::LOAD,1,0,-1,false},{archpulse::OpCode::ADD,2,1,3,false},{archpulse::OpCode::BRANCH,-1,2,-1,false},{archpulse::OpCode::MUL,4,2,5,false}};archpulse::PipelineSimulator sim(archpulse::PredictorKind::AlwaysTaken);auto s=sim.run(prog);assert(s.instructions==4&&s.data_hazards>=2&&s.branches==1&&s.mispredictions==1);
  archpulse::ThreadPool pool(2);auto a=pool.submit([]{return 20;});auto b=pool.submit([](int x){return x*2;},11);assert(a.get()==20&&b.get()==22);
  std::cout<<"All ArchPulse tests passed.\n";
}
