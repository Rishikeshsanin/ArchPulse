#include "archpulse/coherence.hpp"
#include <stdexcept>

namespace archpulse {

double CoherenceStats::read_miss_rate() const {
  return reads ? 100.0 * static_cast<double>(read_misses) / reads : 0.0;
}

double CoherenceStats::write_miss_rate() const {
  return writes ? 100.0 * static_cast<double>(write_misses) / writes : 0.0;
}

MesiSimulator::MesiSimulator(std::size_t cores, std::size_t line_size_bytes)
    : cores_(cores), line_size_bytes_(line_size_bytes) {
  if (cores_ == 0 || line_size_bytes_ == 0)
    throw std::invalid_argument("MESI simulator requires non-zero cores and line size");
}

std::uint64_t MesiSimulator::line_of(std::uint64_t address) const {
  return address / line_size_bytes_;
}

MesiSimulator::StateVector& MesiSimulator::states_for(std::uint64_t line) {
  auto [it, inserted] = lines_.try_emplace(line, cores_, MesiState::Invalid);
  (void)inserted;
  return it->second;
}

void MesiSimulator::validate_core(std::size_t core) const {
  if (core >= cores_) throw std::out_of_range("core index out of range");
}

void MesiSimulator::set_state(StateVector& states, std::size_t core, MesiState next) {
  if (states[core] != next) {
    states[core] = next;
    ++stats_.state_transitions;
  }
}

bool MesiSimulator::read(std::size_t core, std::uint64_t address) {
  validate_core(core);
  ++stats_.reads;
  auto& states = states_for(line_of(address));
  if (states[core] != MesiState::Invalid) return true;

  ++stats_.read_misses;
  ++stats_.bus_reads;
  bool shared = false;
  for (std::size_t i = 0; i < cores_; ++i) {
    if (i == core) continue;
    if (states[i] == MesiState::Modified) {
      ++stats_.writebacks;
      set_state(states, i, MesiState::Shared);
      shared = true;
    } else if (states[i] == MesiState::Exclusive) {
      set_state(states, i, MesiState::Shared);
      shared = true;
    } else if (states[i] == MesiState::Shared) {
      shared = true;
    }
  }
  set_state(states, core, shared ? MesiState::Shared : MesiState::Exclusive);
  return false;
}

bool MesiSimulator::write(std::size_t core, std::uint64_t address) {
  validate_core(core);
  ++stats_.writes;
  auto& states = states_for(line_of(address));

  if (states[core] == MesiState::Modified) return true;
  if (states[core] == MesiState::Exclusive) {
    set_state(states, core, MesiState::Modified);
    return true;
  }

  if (states[core] == MesiState::Shared) {
    for (std::size_t i = 0; i < cores_; ++i) {
      if (i != core && states[i] != MesiState::Invalid) {
        set_state(states, i, MesiState::Invalid);
        ++stats_.invalidations;
      }
    }
    set_state(states, core, MesiState::Modified);
    return true;
  }

  ++stats_.write_misses;
  ++stats_.bus_read_exclusive;
  for (std::size_t i = 0; i < cores_; ++i) {
    if (i == core || states[i] == MesiState::Invalid) continue;
    if (states[i] == MesiState::Modified) ++stats_.writebacks;
    set_state(states, i, MesiState::Invalid);
    ++stats_.invalidations;
  }
  set_state(states, core, MesiState::Modified);
  return false;
}

MesiState MesiSimulator::state(std::size_t core, std::uint64_t address) const {
  validate_core(core);
  const auto it = lines_.find(line_of(address));
  if (it == lines_.end()) return MesiState::Invalid;
  return it->second[core];
}

void MesiSimulator::reset() {
  lines_.clear();
  stats_ = {};
}

std::string to_string(MesiState state) {
  switch (state) {
    case MesiState::Invalid: return "I";
    case MesiState::Shared: return "S";
    case MesiState::Exclusive: return "E";
    case MesiState::Modified: return "M";
  }
  return "?";
}

}  // namespace archpulse
