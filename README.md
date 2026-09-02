# ArchPulse

## CPU & Heterogeneous Compute Performance Workbench

ArchPulse is a **C++20 systems-performance workbench** for exploring computer architecture, CPU behavior, parallel execution, and performance optimization. It combines configurable cache simulation, a five-stage CPU pipeline model, branch-prediction analysis, multithreaded compute benchmarks, Python-based performance visualization, and an interactive browser companion.

> **simulate -> benchmark -> measure -> analyze**

### Highlights

- Configurable CPU cache simulator with cache size, block size, associativity, **LRU replacement**, hit/miss/eviction statistics, and estimated memory cycles.
- Simplified five-stage pipeline model: `IF -> ID -> EX -> MEM -> WB`, including RAW dependencies, load-use penalties, stalls, **CPI** and **IPC**.
- Branch prediction laboratory with always-taken, always-not-taken, **1-bit** and **2-bit saturating** predictors.
- Custom **C++20 thread pool** using `std::thread`, mutexes, condition variables, futures, and a synchronization-safe task queue.
- Parallel **matrix multiplication** and **Gaussian image-convolution** workloads with runtime, throughput, and scaling measurements.
- CSV metrics plus **Python / Pandas / Matplotlib** analytics.
- Interactive **ArchPulse Web Lab** for cache, pipeline, branch-prediction, and parallel-scaling experiments in the browser.
- CTest unit tests and **GitHub Actions CI on Linux and Windows**.

### Build

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

### Cache experiment

```bash
./build/archpulse cache --accesses 100000 --cache-size 32768 --block-size 64 --ways 8 --pattern mixed --csv results/cache.csv
```

### Pipeline experiment

```bash
./build/archpulse pipeline --instructions 50000 --predictor 2bit --csv results/pipeline.csv
```

Predictors: `taken`, `not-taken`, `1bit`, `2bit`.

### Parallel benchmark

```bash
./build/archpulse benchmark --workload matrix --size 256 --threads 8 --csv results/benchmark.csv
./build/archpulse benchmark --workload convolution --size 2048 --threads 8
```

### Scaling study

```bash
./build/archpulse scale --size 256 --max-threads 8 --csv results/benchmark.csv
python -m pip install -r analytics/requirements.txt
python analytics/analyze.py results
```

Runtime and speedup are measured on the machine running ArchPulse; the project intentionally avoids hard-coded performance claims. The browser Scaling Lab is explicitly an analytical model, not a hardware benchmark.

### Engineering concepts

`C++20` · `Computer Architecture` · `Computer Organization` · `Data Structures & Algorithms` · `Operating Systems Concepts` · `Multithreading` · `Concurrent Programming` · `CPU Caches` · `LRU` · `CPU Pipelines` · `Branch Prediction` · `Performance Benchmarking` · `CMake` · `Python` · `Pandas` · `Matplotlib` · `Software Testing`

### Current scope

Implemented: cache/LRU simulation, pipeline/hazard model, four branch predictors, custom thread pool, matrix/convolution benchmarks, scaling measurements, CSV export, Python charts, tests, CI, and the interactive Web Lab.

Planned: OpenCL GPU kernels and CPU-vs-GPU benchmarking, multi-level caches, SIMD paths, trace ingestion, hardware performance counters, and ML-based runtime prediction.

> The simulator is an educational architecture model, not a cycle-accurate model of a proprietary commercial processor.
