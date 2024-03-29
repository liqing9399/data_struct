/***************************************************
#filename      : layernorm.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-01 18:15:45
#last modified : 2023-06-08 14:23:02
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
      : BaseLayer({in}, name) {
    auto out = Tensor::Create(in->GetShape(), in->GetDtype(), OpOutName(name));
    PushOutput(out);
  }

  void Check() override { }

  uint64_t ForwardEvalCycle() override {
    auto in = GetInput(0);
    int lh = in->GetShape().GetExcludeLastDimCount();
    int lw = in->GetShape().GetLastDim();;
    uint64_t vme_cycle = VmePerfEval(lh, lw, lh, lw);
    perf_.cycles_vme = vme_cycle * (14 + 2);  // 14 vme + 2 meacc
    perf_.cycles_gdram2l1 = TransDataPerfEval(lh, lw, in->GetDataWidths(), TransferChannel::GDRAM_L1);
    perf_.cycles_l12gdram = TransDataPerfEval(lh, lw, in->GetDataWidths(), TransferChannel::GDRAM_L1);
    perf_.UpdateBottleneck();
    perf_.PrintInfo(name_);
    return perf_.GetBottleneck();
  }

  uint64_t BackwardEvalCycle() override {
    return 0;
  }
};

}  // namespace nnte
#endif // NNTE_INCLUDE_OPS_LAYERNORM_H_

