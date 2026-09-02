#include "archpulse/metrics.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>
namespace archpulse {
namespace {
bool header(const std::filesystem::path& p,bool a){return !a||!std::filesystem::exists(p)||std::filesystem::file_size(p)==0;}
void parent(const std::filesystem::path& p){if(p.has_parent_path())std::filesystem::create_directories(p.parent_path());}
}
void write_benchmark_csv(const std::filesystem::path& p,const BenchmarkResult& r,bool a){parent(p);bool h=header(p,a);std::ofstream o(p,a?std::ios::app:std::ios::trunc);if(!o)throw std::runtime_error("could not open CSV");if(h)o<<"workload,problem_size,threads,runtime_ms,throughput,speedup,verified\n";o<<r.workload<<','<<r.problem_size<<','<<r.threads<<','<<r.runtime_ms<<','<<r.throughput<<','<<r.speedup<<','<<(r.verified?1:0)<<'\n';}
void write_cache_csv(const std::filesystem::path& p,const CacheConfig& c,const CacheStats& s,bool a){parent(p);bool h=header(p,a);std::ofstream o(p,a?std::ios::app:std::ios::trunc);if(!o)throw std::runtime_error("could not open CSV");if(h)o<<"cache_size,block_size,ways,accesses,hits,misses,evictions,hit_rate,miss_rate,estimated_cycles\n";o<<c.cache_size_bytes<<','<<c.block_size_bytes<<','<<c.ways<<','<<s.accesses<<','<<s.hits<<','<<s.misses<<','<<s.evictions<<','<<s.hit_rate()<<','<<s.miss_rate()<<','<<s.estimated_cycles<<'\n';}
void write_pipeline_csv(const std::filesystem::path& p,PredictorKind k,const PipelineStats& s,bool a){parent(p);bool h=header(p,a);std::ofstream o(p,a?std::ios::app:std::ios::trunc);if(!o)throw std::runtime_error("could not open CSV");if(h)o<<"predictor,instructions,cycles,cpi,ipc,data_hazards,stalls,branches,correct_predictions,mispredictions,prediction_accuracy\n";o<<to_string(k)<<','<<s.instructions<<','<<s.cycles<<','<<s.cpi()<<','<<s.ipc()<<','<<s.data_hazards<<','<<s.stalls<<','<<s.branches<<','<<s.correct_predictions<<','<<s.mispredictions<<','<<s.prediction_accuracy()<<'\n';}
std::string platform_name(){
#if defined(_WIN32)
return "Windows";
#elif defined(__APPLE__)
return "macOS";
#elif defined(__linux__)
return "Linux";
#else
return "Unknown";
#endif
}
}
