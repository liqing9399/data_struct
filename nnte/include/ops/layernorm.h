/***************************************************
#filename      : layernorm.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-01 18:15:45
#last modified : 2023-06-05 16:18:07
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_OPS_LAYERNORM_H_
#define NNTE_INCLUDE_OPS_LAYERNORM_H_


#include "../logger.h"
#include "../common.h"

namespace nnte {
class LayerNormLayer : public BaseLayer {
 public:
  static Sptr<LayerNormLayer> Create(Sptr<Tensor> in,
                                     std::string name = "LayerNormLayer") {
    return std::make_shared<LayerNormLayer>(in, name);
  }

  LayerNormLayer(Sptr<Tensor> in,
                 std::string &name)
      : BaseLayer({in}, {}, name) {
    auto out = Tensor::Create(in->GetShape(), in->GetDtype(), OpOutName(name));
    PushOutput(out);
  }

  void Check() override { }

  uint64_t EvaluateCycle() override {
    int lh = GetInputs()[0]->GetShape().GetExcludeLastDimCount();
    int lw = GetInputs()[0]->GetShape().GetLastDimCount();;
    uint64_t vme_cycle = VmePerfEval(lh, lw, lh, lw);
    uint64_t sum_cycle = vme_cycle * 12;
    return sum_cycle;
  }
};

}  // namespace nnte
#endif // NNTE_INCLUDE_OPS_LAYERNORM_H_

