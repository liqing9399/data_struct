/***************************************************
#filename      : matmul.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-01 14:47:36
#last modified : 2023-06-05 20:30:55
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_OPS_MATMUL_H_
#define NNTE_INCLUDE_OPS_MATMUL_H_


#include <utility>
#include "../logger.h"
#include "../common.h"
#include "../platform.h"


namespace nnte {

class MatmulParam : public BaseParam {
 public:
  MatmulParam() : active_(ActiveMode::AM_NONE) {};
  MatmulParam(ActivationMode am) : active_(am) {};
  MatmulParam(const MatmulParam &p) : active_(p.active_) {};
  MatmulParam &operator=(const MatmulParam &rhs) {
    this->active_ = rhs.active_;
    return *this;
  }
  void PrintInfo() override {
    std::stringstream infos;
    std::string at = ActivationMode2String(active_);
    PVSTR(infos, at);
  }

 public:
  ActiveMode active_;
};

class MatmulLayer : public BaseLayer {
 public:
  static Sptr<MatmulLayer> Create(Sptr<Tensor> left,
                                  Sptr<Tensor> right,
                                  Sptr<Tensor> bias,
                                  MatmulParam param,
                                  std::string name = "MatmulLayer") {
    return std::make_shared<MatmulLayer>(left, right, bias, param, name);
  }

  MatmulLayer(Sptr<Tensor> left,
              Sptr<Tensor> right,
              Sptr<Tensor> bias,
              MatmulParam param,
              std::string &name)
      : BaseLayer({left, right}, {}, name), mm_param_(param) {
    if (bias) PushInput(bias);
    Check();

    Dims d = left->GetShape().GetDims();
    d[d.size() - 1] =  right->GetShape().GetDim(Axis::DIMW);
    Shape s = {d, left->GetShape().GetLayout()};
    auto out = Tensor::Create(s, left->GetDtype(), OpOutName(name));
    PushOutput(out);
  }

  void Check() override {
    int lw = 1;
    int rh = 1;
    int rw = 1;
    int dims_w = GetInputs()[1]->GetShape().GetDimNum();
    // check shape of tensors is matched
    if (dims_w != 2) {
      infos_ << "matmul need right tensor dimension is 2";
      PVSTR(infos_, dims_w);
      throw name_ + " error!";
    }
    lw = GetInputs()[0]->GetShape().GetDim(Axis::DIMW);
    rh = GetInputs()[1]->GetShape().GetDim(Axis::DIMH);
    rw = GetInputs()[1]->GetShape().GetDim(Axis::DIMW);
    bool is_matched = 1;
    std::stringstream infos;
    infos << "matmul's operand shape not matched: ";
    if (lw != rh) {
      PVSTR(infos, lw);
      PVSTR(infos, rh);
      is_matched = 0;
    }

    if (GetInputs().size() == 3) {
      int bw = GetInputs()[2]->GetShape().GetDim(0);
      if (bw != rw) {
        PVSTR(infos, rw);
        PVSTR(infos, bw);
        is_matched = 0;
      }
    }
    if (!is_matched) {
      LOG(E) << infos.str();
      throw name_ + " has error.";
    }
  }

  uint64_t EvaluateCycleV2() {
    int lh = GetInputs()[0]->GetShape().GetDim(Axis::DIMH);
    int lw = GetInputs()[0]->GetShape().GetDim(Axis::DIMW);
    int rw = GetInputs()[1]->GetShape().GetDim(Axis::DIMW);
    perf_.cycles_mme = MmePerfEval(lh, lw, rw);
    perf_.cycles_gdram2l1 = (uint64_t)(lh * lw * SizeOf(GetInputs()[0]->GetDtype()))
                            / transfer_bandwidth[GetPlatform()][TransferChannel::GDRAM_L1];
    perf_.cycles_gdram2llb = (uint64_t)(lw * rw * SizeOf(GetInputs()[1]->GetDtype()))
                            / transfer_bandwidth[GetPlatform()][TransferChannel::GDRAM_LLB];
    perf_.cycles_llb2l1_b = (uint64_t)(lw * rw * SizeOf(GetInputs()[1]->GetDtype()))
                            / transfer_bandwidth[GetPlatform()][TransferChannel::LLB_L1_B];
    perf_.cycles_l12gdram = (uint64_t)(lh * rw * SizeOf(GetOutputs()[0]->GetDtype()))
                            / transfer_bandwidth[GetPlatform()][TransferChannel::GDRAM_L1];
    perf_.UpdateBottleneck();
    perf_.PrintInfo(name_);
    return perf_.GetBottleneck();
  }

  uint64_t EvaluateCycle() override {
    uint64_t cycles = EvaluateCycleV2();
    return cycles;
  }

  // workspace size, the operation needs extra DDR memory
  MatmulParam mm_param_;
};

}

#endif // NNTE_INCLUDE_OPS_MATMUL_H_

