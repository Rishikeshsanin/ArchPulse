#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace archpulse {
enum class ReplacementPolicy { LRU };
struct CacheConfig { std::size_t cache_size_bytes{32*1024}; std::size_t block_size_bytes{64}; std::size_t ways{8}; std::uint64_t hit_latency_cycles{4}; std::uint64_t miss_latency_cycles{100}; ReplacementPolicy policy{ReplacementPolicy::LRU}; };
struct CacheStats { std::uint64_t accesses{0}, hits{0}, misses{0}, evictions{0}, estimated_cycles{0}; [[nodiscard]] double hit_rate() const; [[nodiscard]] double miss_rate() const; };
class CacheSimulator { public: explicit CacheSimulator(CacheConfig config); bool access(std::uint64_t address); void reset(); [[nodiscard]] const CacheConfig& config() const { return config_; } [[nodiscard]] const CacheStats& stats() const { return stats_; } [[nodiscard]] std::size_t set_count() const { return sets_.size(); } private: struct Line { bool valid{false}; std::uint64_t tag{0}; std::uint64_t last_used{0}; }; CacheConfig config_; std::vector<std::vector<Line>> sets_; CacheStats stats_{}; std::uint64_t clock_{0}; };
std::string to_string(ReplacementPolicy policy);
}
