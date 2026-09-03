#include "archpulse/tlb.hpp"
#include <algorithm>
#include <stdexcept>
namespace archpulse {
double TlbStats::hit_rate() const { return accesses ? 100.0 * static_cast<double>(hits) / accesses : 0.0; }
double TlbStats::miss_rate() const { return accesses ? 100.0 * static_cast<double>(misses) / accesses : 0.0; }
TlbSimulator::TlbSimulator(TlbConfig config) : config_(config) {
  if (config_.entries == 0 || config_.ways == 0 || config_.page_size_bytes == 0 || config_.entries % config_.ways != 0)
    throw std::invalid_argument("TLB entries must be non-zero and divisible by ways");
  sets_.resize(config_.entries / config_.ways, std::vector<Entry>(config_.ways));
}
bool TlbSimulator::access(std::uint64_t virtual_address) {
  ++clock_; ++stats_.accesses;
  const std::uint64_t page = virtual_address / config_.page_size_bytes;
  const std::size_t set_index = static_cast<std::size_t>(page % sets_.size());
  const std::uint64_t tag = page / sets_.size();
  auto& set = sets_[set_index];
  for (auto& entry : set) {
    if (entry.valid && entry.tag == tag) {
      ++stats_.hits; stats_.estimated_cycles += config_.hit_latency_cycles; entry.last_used = clock_; return true;
    }
  }
  ++stats_.misses; stats_.estimated_cycles += config_.hit_latency_cycles + config_.miss_penalty_cycles;
  auto victim = std::find_if(set.begin(), set.end(), [](const Entry& e){ return !e.valid; });
  if (victim == set.end()) {
    victim = std::min_element(set.begin(), set.end(), [](const Entry& a, const Entry& b){ return a.last_used < b.last_used; });
    ++stats_.evictions;
  }
  victim->valid = true; victim->tag = tag; victim->last_used = clock_; return false;
}
void TlbSimulator::reset() {
  stats_ = {}; clock_ = 0;
  for (auto& set : sets_) for (auto& entry : set) entry = {};
}
}
