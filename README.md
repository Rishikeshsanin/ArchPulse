# ArchPulse Observatory

### CPU & Heterogeneous Compute Performance Workbench

[![CI](https://github.com/Rishikeshsanin/ArchPulse/actions/workflows/ci.yml/badge.svg)](https://github.com/Rishikeshsanin/ArchPulse/actions/workflows/ci.yml)
[![Live Observatory](https://img.shields.io/badge/Live-ArchPulse%20Observatory-a8ff35)](https://archpulse-six.vercel.app)
![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C)
![Python](https://img.shields.io/badge/Python-Analytics-3776AB)

**ArchPulse** is a systems-performance engineering project that combines a native **C++20 simulation/benchmark engine**, Python analytics, and an interactive browser-based **Performance Observatory**. It is built to explore the concepts that determine real compute performance: cache locality, TLB behavior, CPU pipelines, branch prediction, multithreading, scaling efficiency, and heterogeneous CPU/GPU execution.

> **simulate → benchmark → measure → compare → optimize**

## Live Observatory

**https://archpulse-six.vercel.app**

The Observatory is not a decorative landing page. It contains six interactive engineering labs:

- **Memory Hierarchy Lab** — configurable L1/L2 caches, TLB pressure, LRU behavior, locality patterns, cache-set heatmaps, hierarchy flow, modeled AMAT, and worker-backed simulation.
- **Pipeline Lab** — five-stage `IF → ID → EX → MEM → WB` model, hazards, stalls, CPI/IPC, predictor comparison, and cycle timeline.
- **Trace Studio** — paste or generate custom memory/instruction traces and inspect cache results plus pipeline notes operation-by-operation.
- **GPU Compute Lab** — runs a **real browser benchmark** comparing JavaScript CPU vector addition against WebGPU compute when WebGPU is available, with output verification.
- **Scaling Lab** — Amdahl and Gustafson models, parallel efficiency, serial-floor analysis, and engineering recommendations.
- **Run Vault** — saves experiment configuration/metrics locally, supports replay, deletion, and reproducible JSON export.

The site also includes workload presets, keyboard controls, an optimization advisor, Web Worker execution, responsive visualizations, share support, and an offline-capable PWA shell.

## Native C++20 Engine

The native project is where ArchPulse performs its systems-oriented experiments.

### Memory subsystem

- Set-associative cache simulator
- Configurable cache size, block size, and associativity
- LRU replacement
- Hit/miss/eviction statistics
- **Set-associative TLB simulator** with configurable page size and associativity
- **L1 + L2 + TLB memory hierarchy simulator**
- Modeled hierarchy latency / average access time
- Sequential, stride, hot-set, mixed-locality, and random workloads

### CPU pipeline

- Simplified five-stage pipeline: `IF → ID → EX → MEM → WB`
- RAW dependency detection and load-use penalties
- Multiply latency and pipeline stalls
- CPI and IPC
- Branch penalties and prediction statistics
- Predictors:
  - Always Taken
  - Always Not Taken
  - 1-bit
  - 2-bit saturating
  - **GShare-style predictor with 8-bit global history and 2-bit counters**

### Parallel benchmark engine

- Custom C++ thread pool
- `std::thread`, mutexes, condition variables, futures, and synchronized task queues
- Parallel matrix multiplication
- Parallel Gaussian convolution
- Runtime, throughput, verification, and thread-scaling studies

### Analytics and engineering

- Structured CSV output
- Python + Pandas + Matplotlib reporting
- CMake / CTest
- GitHub Actions on **Linux and Windows**
- No hard-coded native performance claims — benchmark numbers come from the machine executing ArchPulse

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

### Native parallel benchmark

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
            ┌────────────────┴────────────────┐
            │                                 │
      Native C++20 Core                 Web Observatory
            │                                 │
   ┌────────┼─────────┐              ┌────────┼─────────┐
   │        │         │              │        │         │
 Cache   Pipeline   TLB/Memory     Workers   Trace    WebGPU
   │        │         │              Lab     Studio    Compute
   └────────┴────┬────┘               │        │         │
                 │                    └────────┴────┬────┘
           Benchmark Engine                       │
                 │                                │
              CSV Metrics                   Run Vault / Advisor
                 │
         Python Analytics
```

## Modeled vs measured

ArchPulse deliberately separates **architecture models** from **measurements**:

- Cache/TLB/pipeline/AMAT and Amdahl/Gustafson results are educational models.
- Native C++ benchmark runtime is measured on the machine running the CLI.
- Browser CPU/WebGPU time is measured with browser timers and explicitly described as browser end-to-end timing.
- The project does **not** claim cycle-accurate behavior for any proprietary commercial processor.

That distinction is intentional: good performance engineering starts with knowing what was simulated and what was actually measured.

## Tech stack

`C++20` · `CMake` · `CTest` · `Python` · `Pandas` · `Matplotlib` · `Multithreading` · `Computer Architecture` · `Cache Simulation` · `TLB` · `Branch Prediction` · `GShare` · `Web Workers` · `WebGPU` · `PWA` · `GitHub Actions` · `Vercel`

## Roadmap

- Native trace-file ingestion
- SIMD benchmark paths
- Hardware performance-counter adapters where the host OS permits them
- Optional OpenCL backend
- Multi-core coherence experiments
- More replacement/prefetch policies

---

Built as an educational performance-engineering workbench for exploring how software, architecture, memory behavior, and parallel execution interact.
