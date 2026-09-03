#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace archpulse {

enum class MesiState { Invalid, Shared, Exclusive, Modified };

struct CoherenceStats {
  std::uint64_t reads{0};
  std::uint64_t writes{0};
  std::uint64_t read_misses{0};
  std::uint64_t write_misses{0};
  std::uint64_t invalidations{0};
  std::uint64_t writebacks{0};
  std::uint64_t bus_reads{0};
  std::uint64_t bus_read_exclusive{0};
  std::uint64_t state_transitions{0};

  [[nodiscard]] double read_miss_rate() const;
  [[nodiscard]] double write_miss_rate() const;
};

class MesiSimulator {
 public:
  explicit MesiSimulator(std::size_t cores = 4, std::size_t line_size_bytes = 64);

  bool read(std::size_t core, std::uint64_t address);
  bool write(std::size_t core, std::uint64_t address);
  void reset();

  [[nodiscard]] MesiState state(std::size_t core, std::uint64_t address) const;
  [[nodiscard]] const CoherenceStats& stats() const { return stats_; }
  [[nodiscard]] std::size_t cores() const { return cores_; }
  [[nodiscard]] std::size_t tracked_lines() const { return lines_.size(); }

 private:
  using StateVector = std::vector<MesiState>;
  std::size_t cores_;
  std::size_t line_size_bytes_;
  std::unordered_map<std::uint64_t, StateVector> lines_;
  CoherenceStats stats_{};

  [[nodiscard]] std::uint64_t line_of(std::uint64_t address) const;
  StateVector& states_for(std::uint64_t line);
  void set_state(StateVector& states, std::size_t core, MesiState next);
  void validate_core(std::size_t core) const;
};

std::string to_string(MesiState state);

}  // namespace archpulse
