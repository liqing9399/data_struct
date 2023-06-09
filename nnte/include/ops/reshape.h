/***************************************************
#filename      : reshape.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-01 15:01:34
#last modified : 2023-06-08 11:33:42
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_OPS_RESHAPE_
#define NNTE_INCLUDE_OPS_RESHAPE_

#include "../logger.h"
#include "../common.h"


namespace nnte {
class ReshapeLayer : public BaseLayer {
 public:
  static Sptr<ReshapeLayer> Create(Sptr<Tensor> in,
                                   Dims dims,
                                   std::string name = "ReshapeLayer") {
    return std::make_shared<ReshapeLayer>(in, dims, name);
  }

  ReshapeLayer(Sptr<Tensor> in, Dims dims, std::string &name) :
    BaseLayer({in}, name), dims_(dims) {
    Check();
    Shape s = {dims, in->GetShape().GetLayout()};
    auto out = Tensor::Create(s, in->GetDtype(), OpOutName(name));
    PushOutput(out);
  }

  void Check() override {
    int in_count = GetInputs()[0]->GetShape().GetCount();
    int out_count = std::accumulate(dims_.begin(), dims_.end(), 1, std::multiplies<int>());
    if (in_count != out_count) {
      std::stringstream infos;
      infos << name_ << " tensor shape not match:\n";
      PVSTR(infos, in_count);
      PVSTR(infos, out_count);
      LOG(E) << infos.str();
      throw "error!";
    }
  }

  uint64_t ForwardEvalCycle() override {
    perf_.PrintInfo(name_);
    return perf_.GetBottleneck();
  }
  uint64_t BackwardEvalCycle() override {
    return 0;
  }

 public:
  Dims dims_;
};

}
#endif // NNTE_INCLUDE_OPS_RESHAPE_

