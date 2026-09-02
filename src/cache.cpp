#include "archpulse/cache.hpp"
#include <algorithm>
#include <stdexcept>
namespace archpulse {
double CacheStats::hit_rate() const { return accesses==0?0.0:100.0*static_cast<double>(hits)/static_cast<double>(accesses); }
double CacheStats::miss_rate() const { return accesses==0?0.0:100.0*static_cast<double>(misses)/static_cast<double>(accesses); }
CacheSimulator::CacheSimulator(CacheConfig config):config_(config){ if(config_.cache_size_bytes==0||config_.block_size_bytes==0||config_.ways==0) throw std::invalid_argument("cache size, block size, and ways must be non-zero"); const auto bps=config_.block_size_bytes*config_.ways; if(config_.cache_size_bytes%bps!=0) throw std::invalid_argument("cache size must be divisible by block_size * ways"); const auto n=config_.cache_size_bytes/bps; if(n==0) throw std::invalid_argument("cache configuration produces zero sets"); sets_.assign(n,std::vector<Line>(config_.ways)); }
bool CacheSimulator::access(std::uint64_t address){ ++clock_; ++stats_.accesses; const auto block=address/config_.block_size_bytes; const auto si=static_cast<std::size_t>(block%sets_.size()); const auto tag=block/sets_.size(); auto& set=sets_[si]; for(auto& line:set){ if(line.valid&&line.tag==tag){ ++stats_.hits; stats_.estimated_cycles+=config_.hit_latency_cycles; line.last_used=clock_; return true; }} ++stats_.misses; stats_.estimated_cycles+=config_.miss_latency_cycles; auto target=std::find_if(set.begin(),set.end(),[](const Line& l){return !l.valid;}); if(target==set.end()){ target=std::min_element(set.begin(),set.end(),[](const Line&a,const Line&b){return a.last_used<b.last_used;}); ++stats_.evictions; } target->valid=true; target->tag=tag; target->last_used=clock_; return false; }
void CacheSimulator::reset(){ for(auto& set:sets_) for(auto& line:set) line=Line{}; stats_=CacheStats{}; clock_=0; }
std::string to_string(ReplacementPolicy p){ return p==ReplacementPolicy::LRU?"LRU":"Unknown"; }
}
