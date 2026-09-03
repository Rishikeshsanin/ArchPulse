# ArchPulse Observatory

### CPU & Heterogeneous Compute Performance Workbench

[![CI](https://github.com/Rishikeshsanin/ArchPulse/actions/workflows/ci.yml/badge.svg)](https://github.com/Rishikeshsanin/ArchPulse/actions/workflows/ci.yml)
[![Live Observatory](https://img.shields.io/badge/Live-ArchPulse%20Observatory-a8ff35)](https://archpulse-six.vercel.app)
![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C)
![Python](https://img.shields.io/badge/Python-Analytics-3776AB)

**ArchPulse** is a systems-performance engineering workbench combining a native **C++20 architecture/benchmark engine**, Python analytics, and an interactive browser **Performance Observatory**. It explores the mechanisms that determine compute performance: cache locality, TLB behavior, multi-core cache coherence, CPU pipelines, branch prediction, multithreading, scaling efficiency, and heterogeneous CPU/GPU execution.

> **simulate → benchmark → measure → compare → optimize**

## Live Observatory

**https://archpulse-six.vercel.app**

The live Observatory contains six interactive engineering labs:

- **Memory Hierarchy Lab** — configurable L1/L2 caches, TLB pressure, LRU behavior, locality patterns, cache-set heatmaps, hierarchy flow and modeled AMAT.
- **Pipeline Lab** — five-stage `IF → ID → EX → MEM → WB` model, hazards, stalls, CPI/IPC, five branch predictors and a cycle timeline.
- **Trace Studio** — paste or generate memory/instruction traces and inspect cache results plus pipeline notes operation-by-operation.
- **GPU Compute Lab** — performs a real browser CPU benchmark and uses WebGPU compute when supported, including result verification.
- **Scaling Lab** — Amdahl and Gustafson models, parallel efficiency and serial-floor analysis.
- **Run Vault** — saves experiment configurations/metrics locally and supports JSON export.

The repository's full Observatory source additionally includes Web Worker execution, workload presets, keyboard controls, share support, an optimization advisor, and an offline-capable PWA shell.

## Native C++20 Engine

### Memory subsystem

- Set-associative cache simulator with configurable cache/block/associativity parameters
- LRU replacement with hit/miss/eviction statistics
- Set-associative **TLB simulator** with configurable page size and associativity
- **L1 + L2 + TLB memory hierarchy simulator**
- Sequential, stride, hot-set, mixed-locality and random workloads
- Modeled hierarchy latency / average access time

### Multi-core MESI coherence

ArchPulse now contains a dedicated **MESI cache-coherence engine** for shared-memory multi-core experiments.

- Per-core cache-line states: **Modified / Exclusive / Shared / Invalid**
- Read and write miss accounting
- Bus read (`BusRd`) and read-exclusive (`BusRdX`) traffic
- Cross-core invalidations
- Dirty-line writebacks
- State-transition counts
- Configurable core count and working-set contention
- Dedicated `archpulse_coherence` CLI

Example:

```bash
./build/archpulse_coherence \
  --cores 8 \
  --operations 250000 \
  --write-percent 30 \
  --working-set-lines 4096
```

### CPU pipeline

- Simplified five-stage pipeline: `IF → ID → EX → MEM → WB`
- RAW dependency detection and load-use penalties
- Multiply latency and pipeline stalls
- CPI / IPC
- Branch penalties and prediction statistics
- Predictors:
  - Always Taken
  - Always Not Taken
  - 1-bit
  - 2-bit saturating
  - **GShare-style predictor with 8-bit global history and 256 two-bit counters**

### Parallel benchmark engine

- Custom C++ thread pool
- `std::thread`, mutexes, condition variables, futures and synchronized task queues
- Parallel matrix multiplication
- Parallel Gaussian convolution
- Runtime, throughput, output verification and thread-scaling studies

### Analytics and engineering

- Structured CSV output
- Python + Pandas + Matplotlib reporting
- CMake / CTest
- Dependency-free browser smoke tests
- GitHub Actions builds/tests on **Linux and Windows**
- Web JavaScript syntax + DOM integration checks in CI
- No hard-coded native performance claims — native timings come from the executing machine

## Quick start

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

### Memory hierarchy

```bash
./build/archpulse memory \
  --accesses 100000 \
  --l1-size 32768 \
  --l2-size 524288 \
  --block-size 64 \
  --ways 8 \
  --tlb-entries 64 \
  --pattern hotset
```

### TLB study

```bash
./build/archpulse tlb --entries 64 --ways 4 --page-size 4096 --pattern stride
```

### Pipeline + GShare

```bash
./build/archpulse pipeline --instructions 50000 --predictor gshare --csv results/pipeline.csv
```

### Parallel benchmark

```bash
./build/archpulse benchmark --workload matrix --size 256 --threads 8 --csv results/benchmark.csv
./build/archpulse benchmark --workload convolution --size 2048 --threads 8
./build/archpulse scale --size 256 --max-threads 8 --csv results/scaling.csv
```

### Python analytics

```bash
python -m pip install -r analytics/requirements.txt
python analytics/analyze.py results
```

## Architecture

```text
                              ARCHPULSE
                                  │
              ┌───────────────────┴───────────────────┐
              │                                       │
        Native C++20 Core                      Web Observatory
              │                                       │
   ┌──────────┼───────────┬──────────┐      ┌─────────┼─────────┐
   │          │           │          │      │         │         │
 Cache     Pipeline   TLB/Memory    MESI   Trace     WebGPU   Scaling
   │          │           │          │      Studio   Compute     Lab
   └──────────┴──────┬────┴──────────┘        │         │         │
                     │                        └─────────┴────┬────┘
              Benchmark Engine                            │
                     │                              Run Vault / Advisor
                 CSV Metrics
                     │
              Python Analytics
```

## Modeled vs measured

ArchPulse deliberately separates architecture models from measurements:

- Cache/TLB/pipeline/coherence/AMAT and Amdahl/Gustafson results are educational architecture models.
- Native C++ benchmark runtime is measured on the machine executing the CLI.
- Browser CPU/WebGPU time is measured with browser timers and explicitly described as end-to-end browser timing.
- ArchPulse does **not** claim cycle-accurate behavior for any proprietary commercial processor.

## Tech stack

`C++20` · `CMake` · `CTest` · `Python` · `Pandas` · `Matplotlib` · `Multithreading` · `Computer Architecture` · `L1/L2 Cache` · `TLB` · `MESI` · `Branch Prediction` · `GShare` · `Web Workers` · `WebGPU` · `PWA` · `GitHub Actions` · `Vercel`

## Roadmap

- Native trace-file ingestion
- SIMD benchmark paths
- Hardware performance-counter adapters where the host OS permits them
- Optional OpenCL backend
- Coherence visualizer / false-sharing experiments
- Additional replacement and prefetch policies

---

Built as an educational performance-engineering workbench for exploring how software, architecture, memory behavior, coherence and parallel execution interact.
