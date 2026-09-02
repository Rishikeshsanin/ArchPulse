#pragma once
#include "archpulse/benchmark.hpp"
#include "archpulse/cache.hpp"
#include "archpulse/pipeline.hpp"
#include <filesystem>
#include <string>
namespace archpulse { void write_benchmark_csv(const std::filesystem::path&,const BenchmarkResult&,bool append=true); void write_cache_csv(const std::filesystem::path&,const CacheConfig&,const CacheStats&,bool append=true); void write_pipeline_csv(const std::filesystem::path&,PredictorKind,const PipelineStats&,bool append=true); std::string platform_name(); }
