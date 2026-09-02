#pragma once
#include <cstddef>
#include <string>
#include <vector>
namespace archpulse { struct BenchmarkResult { std::string workload; std::size_t problem_size{0},threads{1}; double runtime_ms{0},throughput{0},speedup{1}; bool verified{false}; }; BenchmarkResult benchmark_matrix_multiplication(std::size_t matrix_size,std::size_t threads,double baseline_ms=0.0); BenchmarkResult benchmark_convolution(std::size_t image_size,std::size_t threads,double baseline_ms=0.0); std::vector<BenchmarkResult> run_scaling_suite(std::size_t problem_size,const std::vector<std::size_t>& thread_counts); }
