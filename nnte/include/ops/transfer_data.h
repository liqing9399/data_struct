/***************************************************
#filename      : transfer_data.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-29 16:33:20
#last modified : 2023-06-05 16:53:57
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
      : BaseLayer({src}, {dst}, name), transfer_channel_(transfer_channel) { }

  void Check() override {}

  uint64_t EvaluateCycle() override {
    auto ss= GetInputs()[0]->GetShape();
    auto ds= GetOutputs()[0]->GetShape();
    int sh = ss.GetExcludeLastDimCount();
    int sw = ss.GetLastDimCount();
    // int s_stride_w
    // int dh = ds.GetExcludeLastDimCount();
    // int dw = ds.GetLastDimCount();
    uint64_t cycles = sh * sw / transfer_bandwidth[GetPlatform()][transfer_channel_];
    LOG(E) << "TransferDataLayer use cycles: " << cycles;
    return cycles;
  }

 private:
  TransferChannel transfer_channel_;
};

}  // namespace nnte
# endif // NNTE_INCLUDE_OPS_TRANSFER_DATA_H_
