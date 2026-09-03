#pragma once
#include "archpulse/cache.hpp"
#include "archpulse/tlb.hpp"
#include <cstdint>
namespace archpulse {
struct MemoryHierarchyConfig {
  CacheConfig l1{};
  CacheConfig l2{512 * 1024, 64, 8, 12, 120, ReplacementPolicy::LRU};
  TlbConfig tlb{};
  std::uint64_t dram_latency_cycles{120};
};
struct MemoryHierarchyStats {
  std::uint64_t accesses{0}, tlb_hits{0}, tlb_misses{0}, l1_hits{0}, l1_misses{0}, l2_hits{0}, l2_misses{0}, dram_accesses{0}, estimated_cycles{0};
  [[nodiscard]] double l1_hit_rate() const;
  [[nodiscard]] double l2_local_hit_rate() const;
  [[nodiscard]] double tlb_hit_rate() const;
  [[nodiscard]] double average_access_time() const;
};
class MemoryHierarchySimulator {
 public:
  explicit MemoryHierarchySimulator(MemoryHierarchyConfig config = {});
  void access(std::uint64_t virtual_address);
  void reset();
  [[nodiscard]] const MemoryHierarchyStats& stats() const { return stats_; }
  [[nodiscard]] const TlbStats& tlb_stats() const { return tlb_.stats(); }
  [[nodiscard]] const CacheStats& l1_stats() const { return l1_.stats(); }
  [[nodiscard]] const CacheStats& l2_stats() const { return l2_.stats(); }
 private:
  MemoryHierarchyConfig config_;
  CacheSimulator l1_;
  CacheSimulator l2_;
  TlbSimulator tlb_;
  MemoryHierarchyStats stats_{};
};
}
