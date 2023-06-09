/***************************************************
#filename      : transfer_data.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-29 16:33:20
#last modified : 2023-06-08 11:34:25
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_OPS_TRANSFER_DATA_H_
#define NNTE_INCLUDE_OPS_TRANSFER_DATA_H_

#include "../nnte_utils.h"
#include "../logger.h"
#include "../platform.h"
#include "../common.h"

namespace nnte {

class TransferDataLayer : public BaseLayer {
 public:
  static Sptr<TransferDataLayer> Create(Sptr<Tensor> src,
                                        Sptr<Tensor> dst,
                                        TransferChannel transfer_channel,
                                        std::string name = "") {
    return std::make_shared<TransferDataLayer>(src, dst, transfer_channel, name);
  }

  TransferDataLayer(Sptr<Tensor> src,
                    Sptr<Tensor> dst,
                    TransferChannel transfer_channel,
                    std::string &name)
      : BaseLayer({src}, name), transfer_channel_(transfer_channel) {
    PushOutput(dst);
  }

  void Check() override {}

  uint64_t ForwardEvalCycle() override {
    auto ss= GetInputs()[0]->GetShape();
    auto ds= GetOutputs()[0]->GetShape();
    int sh = ss.GetExcludeLastDimCount();
    int sw = ss.GetLastDim();
    // int s_stride_w
    // int dh = ds.GetExcludeLastDimCount();
    // int dw = ds.GetLastDimCount();
    perf_.cycles_gdram2gdram = TransDataPerfEval(sh, sw, (GetInput(0)->GetDataWidths()), transfer_channel_);
    perf_.UpdateBottleneck();
    perf_.PrintInfo(name_);
    return perf_.GetBottleneck();
  }
  uint64_t BackwardEvalCycle() override {
    return 0;
  }

 private:
  TransferChannel transfer_channel_;
};

}  // namespace nnte
# endif // NNTE_INCLUDE_OPS_TRANSFER_DATA_H_
