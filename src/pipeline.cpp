#include "archpulse/pipeline.hpp"
#include <algorithm>
#include <cctype>
#include <random>
#include <stdexcept>
namespace archpulse {
double PipelineStats::cpi() const{return instructions?static_cast<double>(cycles)/instructions:0.0;}
double PipelineStats::ipc() const{return cycles?static_cast<double>(instructions)/cycles:0.0;}
double PipelineStats::prediction_accuracy() const{return branches?100.0*static_cast<double>(correct_predictions)/branches:0.0;}
BranchPredictor::BranchPredictor(PredictorKind k):kind_(k){gshare_table_.fill(2);}
bool BranchPredictor::predict(std::uint64_t branch_id)const{
  switch(kind_){
    case PredictorKind::AlwaysTaken:return true;
    case PredictorKind::AlwaysNotTaken:return false;
    case PredictorKind::OneBit:return one_bit_state_;
    case PredictorKind::TwoBit:return two_bit_state_>=2;
    case PredictorKind::GShare:{const std::size_t index=static_cast<std::size_t>((branch_id^global_history_)&0xffu);return gshare_table_[index]>=2;}
  }
  return false;
}
void BranchPredictor::update(bool taken,std::uint64_t branch_id){
  if(kind_==PredictorKind::OneBit)one_bit_state_=taken;
  else if(kind_==PredictorKind::TwoBit){if(taken)two_bit_state_=static_cast<std::uint8_t>(std::min<int>(3,two_bit_state_+1));else two_bit_state_=static_cast<std::uint8_t>(std::max<int>(0,two_bit_state_-1));}
  else if(kind_==PredictorKind::GShare){
    const std::size_t index=static_cast<std::size_t>((branch_id^global_history_)&0xffu);
    auto& counter=gshare_table_[index];
    if(taken)counter=static_cast<std::uint8_t>(std::min<int>(3,counter+1));else counter=static_cast<std::uint8_t>(std::max<int>(0,counter-1));
    global_history_=static_cast<std::uint8_t>((global_history_<<1)|(taken?1u:0u));
  }
}
void BranchPredictor::reset(){one_bit_state_=true;two_bit_state_=2;global_history_=0;gshare_table_.fill(2);}
PipelineSimulator::PipelineSimulator(PredictorKind p):predictor_kind_(p){}
PipelineStats PipelineSimulator::run(const std::vector<Instruction>& program){PipelineStats s{};if(program.empty())return s;BranchPredictor p(predictor_kind_);s.instructions=program.size();s.cycles=program.size()+4;for(std::size_t i=0;i<program.size();++i){const auto& c=program[i];if(i>0){const auto& prev=program[i-1];if(prev.dst>=0&&(prev.dst==c.src1||prev.dst==c.src2)){++s.data_hazards;++s.stalls;++s.cycles;if(prev.op==OpCode::LOAD){++s.stalls;++s.cycles;}}}if(c.op==OpCode::BRANCH){++s.branches;const bool pred=p.predict(i);if(pred==c.branch_taken)++s.correct_predictions;else{++s.mispredictions;s.stalls+=2;s.cycles+=2;}p.update(c.branch_taken,i);}if(c.op==OpCode::MUL){++s.stalls;++s.cycles;}}return s;}
std::vector<Instruction> generate_program(std::size_t count,std::uint32_t seed){std::mt19937 rng(seed);std::uniform_int_distribution<int> od(0,99),rd(0,15);std::bernoulli_distribution bd(.62),dd(.30);std::vector<Instruction> p;p.reserve(count);int prev=-1;for(std::size_t i=0;i<count;++i){int r=od(rng);Instruction x{};if(r<30)x.op=OpCode::ADD;else if(r<48)x.op=OpCode::SUB;else if(r<58)x.op=OpCode::MUL;else if(r<73)x.op=OpCode::LOAD;else if(r<83)x.op=OpCode::STORE;else x.op=OpCode::BRANCH;x.src1=dd(rng)&&prev>=0?prev:rd(rng);x.src2=rd(rng);x.dst=(x.op==OpCode::STORE||x.op==OpCode::BRANCH)?-1:rd(rng);x.branch_taken=x.op==OpCode::BRANCH?bd(rng):false;prev=x.dst;p.push_back(x);}return p;}
std::string to_string(PredictorKind k){switch(k){case PredictorKind::AlwaysTaken:return"always-taken";case PredictorKind::AlwaysNotTaken:return"always-not-taken";case PredictorKind::OneBit:return"1bit";case PredictorKind::TwoBit:return"2bit";case PredictorKind::GShare:return"gshare";}return"unknown";}
PredictorKind predictor_from_string(const std::string& value){std::string v=value;std::transform(v.begin(),v.end(),v.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});if(v=="taken"||v=="always-taken")return PredictorKind::AlwaysTaken;if(v=="not-taken"||v=="always-not-taken")return PredictorKind::AlwaysNotTaken;if(v=="1bit"||v=="one-bit")return PredictorKind::OneBit;if(v=="2bit"||v=="two-bit")return PredictorKind::TwoBit;if(v=="gshare"||v=="global-history")return PredictorKind::GShare;throw std::invalid_argument("unknown predictor: "+value);}
}
