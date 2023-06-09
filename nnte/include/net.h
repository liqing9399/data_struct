/***************************************************
#filename      : ops.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-29 11:59:55
#last modified : 2023-06-08 11:39:20
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_NET_H_
#define NNTE_INCLUDE_NET_H_

#include <utility>
#include "./logger.h"
#include "./common.h"

#include "./ops/embedding.h"
#include "./ops/matmul.h"
#include "./ops/batchmatmul.h"
#include "./ops/transfer_data.h"
#include "./ops/reshape.h"
#include "./ops/transpose.h"
#include "./ops/layernorm.h"
#include "./ops/binary.h"

namespace nnte {

class Network : public BaseLayer {
 public:
  static std::shared_ptr<Network> Create(std::string name = "NetworkLayer") {
    return std::make_shared<Network>(name);
  }

  explicit Network(std::string &name) : BaseLayer({}, name), sum_cycles_(0) {};
  virtual ~Network(){};

  void Check() override {};

  void AddOperator(Sptr<BaseLayer> op) {
    layers_.push_back(op);
  }
  void AddRepeatedOperator(Sptr<BaseLayer> op, int num) {
    layers_.push_back(op);
    repeated_layers_[op] = num;
  }

  // TODO(LITAO): Now only the calculation order is supported in the same order as add order
  void TopoGraph() {}

  void UpdateCycleRatio() {
    double sum_ratio = 0.0;
    std::stringstream cycle_info;
    for (auto &it : op_cycles_) {
      double ratio = 0.0;
      uint64_t layer_cycle = 0;
      if (repeated_layers_.find(it.first)  != repeated_layers_.end()){
        layer_cycle = it.second.first * repeated_layers_[it.first];
      } else {
        layer_cycle = it.second.first;
      }
      ratio = (double)layer_cycle / sum_cycles_;
      it.second.second = ratio;
      sum_ratio += ratio;
      cycle_info << "summary cycles: " << sum_cycles_ << "(" << cycle2ms(sum_cycles_) << "ms)"
                 << ". layer: " << it.first->GetLayerName()  << ". cycles: " << layer_cycle
                 << "(" << cycle2ms(layer_cycle) << "ms)"
                 << ". ratio: " << ratio << "\n" ;
    }
    LOG(I) << "\n" << cycle_info.str();
    LOG(I) << "summary ratio: " << sum_ratio;
  }

  uint64_t ForwardEvalCycle() override {
    TopoGraph();
    uint64_t cycles = 0;
    for (auto layer : layers_) {
      uint64_t temp = layer->ForwardEvalCycle();
      op_cycles_[layer] = {temp, 0.0};
      int rp_num = 1;
      if (repeated_layers_.find(layer) != repeated_layers_.end()) {
        rp_num = repeated_layers_[layer];
      }
      cycles += (temp * rp_num);
    }
    sum_cycles_ = cycles;
    LOG(I) << "time summary: " << cycles << "(cycle)"
           << " = " << cycle2ms(cycles) << "(ms)"
           << std::endl;
    UpdateCycleRatio();
    return cycles;
  }

  uint64_t BackwardEvalCycle() override {
    return 0;
  }

 private:
  uint64_t sum_cycles_;
  std::vector<Sptr<BaseLayer>> layers_;
  std::map<Sptr<BaseLayer>, int> repeated_layers_;
  std::map<Sptr<BaseLayer>, std::pair<uint64_t, double>> op_cycles_;
};

} // namespace nnte

#endif // NNTE_INCLUDE_NET_H_

