#include "archpulse/memory_hierarchy.hpp"
namespace archpulse {
double MemoryHierarchyStats::l1_hit_rate() const { return accesses ? 100.0 * static_cast<double>(l1_hits) / accesses : 0.0; }
double MemoryHierarchyStats::l2_local_hit_rate() const { return l1_misses ? 100.0 * static_cast<double>(l2_hits) / l1_misses : 0.0; }
double MemoryHierarchyStats::tlb_hit_rate() const { return accesses ? 100.0 * static_cast<double>(tlb_hits) / accesses : 0.0; }
double MemoryHierarchyStats::average_access_time() const { return accesses ? static_cast<double>(estimated_cycles) / accesses : 0.0; }
MemoryHierarchySimulator::MemoryHierarchySimulator(MemoryHierarchyConfig config)
    : config_(config), l1_(config_.l1), l2_(config_.l2), tlb_(config_.tlb) {}
void MemoryHierarchySimulator::access(std::uint64_t virtual_address) {
  ++stats_.accesses;
  const bool tlb_hit = tlb_.access(virtual_address);
  if (tlb_hit) ++stats_.tlb_hits; else ++stats_.tlb_misses;
  stats_.estimated_cycles += tlb_hit ? config_.tlb.hit_latency_cycles : config_.tlb.hit_latency_cycles + config_.tlb.miss_penalty_cycles;
  if (l1_.access(virtual_address)) {
    ++stats_.l1_hits;
    stats_.estimated_cycles += config_.l1.hit_latency_cycles;
    return;
  }
  ++stats_.l1_misses;
  stats_.estimated_cycles += config_.l1.hit_latency_cycles;
  if (l2_.access(virtual_address)) {
    ++stats_.l2_hits;
    stats_.estimated_cycles += config_.l2.hit_latency_cycles;
    return;
  }
  ++stats_.l2_misses;
  ++stats_.dram_accesses;
  stats_.estimated_cycles += config_.l2.hit_latency_cycles + config_.dram_latency_cycles;
}
void MemoryHierarchySimulator::reset() { stats_ = {}; l1_.reset(); l2_.reset(); tlb_.reset(); }
}
