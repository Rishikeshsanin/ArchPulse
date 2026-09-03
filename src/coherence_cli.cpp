#include "archpulse/coherence.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

namespace {
std::size_t value(int argc, char** argv, const std::string& key, std::size_t fallback) {
  for (int i = 1; i + 1 < argc; ++i)
    if (argv[i] == key) return static_cast<std::size_t>(std::stoull(argv[i + 1]));
  return fallback;
}
}

int main(int argc, char** argv) {
  try {
    const std::size_t cores = value(argc, argv, "--cores", 4);
    const std::size_t operations = value(argc, argv, "--operations", 100000);
    const std::size_t write_percent = std::min<std::size_t>(100, value(argc, argv, "--write-percent", 25));
    const std::size_t working_set_lines = std::max<std::size_t>(1, value(argc, argv, "--working-set-lines", 4096));

    archpulse::MesiSimulator sim(cores, 64);
    std::mt19937_64 rng(2026);
    std::uniform_int_distribution<std::size_t> core_dist(0, cores - 1);
    std::uniform_int_distribution<std::size_t> line_dist(0, working_set_lines - 1);
    std::uniform_int_distribution<int> op_dist(0, 99);
    std::bernoulli_distribution shared_hotspot(0.72);

    for (std::size_t i = 0; i < operations; ++i) {
      const std::size_t core = core_dist(rng);
      const std::size_t line = shared_hotspot(rng) ? line_dist(rng) % std::min<std::size_t>(64, working_set_lines) : line_dist(rng);
      const std::uint64_t address = static_cast<std::uint64_t>(line) * 64;
      if (op_dist(rng) < static_cast<int>(write_percent)) sim.write(core, address);
      else sim.read(core, address);
    }

    const auto& s = sim.stats();
    std::cout << "\n============================================================\n"
              << "  ArchPulse MESI Coherence Observatory\n"
              << "============================================================\n"
              << "  Cores: " << cores << " | Operations: " << operations
              << " | Write ratio: " << write_percent << "%\n"
              << "  Tracked cache lines: " << sim.tracked_lines() << "\n\n"
              << "  Reads: " << s.reads << " | Read miss rate: " << std::fixed << std::setprecision(2) << s.read_miss_rate() << "%\n"
              << "  Writes: " << s.writes << " | Write miss rate: " << s.write_miss_rate() << "%\n"
              << "  Invalidations: " << s.invalidations << " | Writebacks: " << s.writebacks << "\n"
              << "  BusRd: " << s.bus_reads << " | BusRdX: " << s.bus_read_exclusive << "\n"
              << "  MESI state transitions: " << s.state_transitions << "\n\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "ArchPulse coherence error: " << e.what() << '\n';
    return 1;
  }
}
