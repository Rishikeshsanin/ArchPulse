#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace archpulse {
enum class OpCode { ADD,SUB,MUL,LOAD,STORE,BRANCH,NOP };
enum class PredictorKind { AlwaysTaken,AlwaysNotTaken,OneBit,TwoBit,GShare };
struct Instruction { OpCode op{OpCode::NOP}; int dst{-1},src1{-1},src2{-1}; bool branch_taken{false}; };
struct PipelineStats { std::uint64_t instructions{0},cycles{0},data_hazards{0},stalls{0},branches{0},correct_predictions{0},mispredictions{0}; [[nodiscard]] double cpi() const; [[nodiscard]] double ipc() const; [[nodiscard]] double prediction_accuracy() const; };
class BranchPredictor {
 public:
  explicit BranchPredictor(PredictorKind kind);
  bool predict(std::uint64_t branch_id = 0) const;
  void update(bool taken, std::uint64_t branch_id = 0);
  void reset();
 private:
  PredictorKind kind_;
  bool one_bit_state_{true};
  std::uint8_t two_bit_state_{2};
  std::uint8_t global_history_{0};
  std::array<std::uint8_t,256> gshare_table_{};
};
class PipelineSimulator { public: explicit PipelineSimulator(PredictorKind predictor); PipelineStats run(const std::vector<Instruction>& program); private: PredictorKind predictor_kind_; };
std::vector<Instruction> generate_program(std::size_t count,std::uint32_t seed=42);
std::string to_string(PredictorKind kind);
PredictorKind predictor_from_string(const std::string& value);
}
