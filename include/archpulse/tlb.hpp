#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
namespace archpulse {
struct TlbConfig {
  std::size_t entries{64};
  std::size_t ways{4};
  std::size_t page_size_bytes{4096};
  std::uint64_t hit_latency_cycles{1};
  std::uint64_t miss_penalty_cycles{20};
};
struct TlbStats {
  std::uint64_t accesses{0}, hits{0}, misses{0}, evictions{0}, estimated_cycles{0};
  [[nodiscard]] double hit_rate() const;
  [[nodiscard]] double miss_rate() const;
};
class TlbSimulator {
 public:
  explicit TlbSimulator(TlbConfig config = {});
  bool access(std::uint64_t virtual_address);
  void reset();
  [[nodiscard]] const TlbConfig& config() const { return config_; }
  [[nodiscard]] const TlbStats& stats() const { return stats_; }
  [[nodiscard]] std::size_t set_count() const { return sets_.size(); }
 private:
  struct Entry { bool valid{false}; std::uint64_t tag{0}; std::uint64_t last_used{0}; };
  TlbConfig config_;
  std::vector<std::vector<Entry>> sets_;
  TlbStats stats_{};
  std::uint64_t clock_{0};
};
}
