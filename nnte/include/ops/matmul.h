/***************************************************
#filename      : matmul.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-01 14:47:36
#last modified : 2023-06-08 14:33:56
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
  explicit MatmulParam(ActivationMode am) : active_(am) {};
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
      : BaseLayer({left, right}, name), mm_param_(param) {
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
    lw = GetInputs()[0]->GetShape().GetLastDim();
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

  uint64_t ForwardEvalCycle() override {
    int lh = GetInputs()[0]->GetShape().GetExcludeLastDimCount();
    int lw = GetInputs()[0]->GetShape().GetLastDim();
    int rw = GetInputs()[1]->GetShape().GetDim(Axis::DIMW);
    MatmulEval(lh, lw, rw, perf_);
    return perf_.GetBottleneck();
  }

  void MatmulEval(int lh, int lw, int rw, CycleCounter &perf) {
    perf.cycles_mme = MmePerfEval(lh, lw, rw);
    perf.cycles_gdram2l1 = TransDataPerfEval(lh, lw, (GetInput(0)->GetDataWidths()), TransferChannel::GDRAM_L1);
    perf.cycles_gdram2llb = TransDataPerfEval(lw, rw, (GetInput(1)->GetDataWidths()), TransferChannel::GDRAM_LLB);
    perf.cycles_llb2l1_b = TransDataPerfEval(lw, rw, (GetInput(1)->GetDataWidths()), TransferChannel::LLB_L1_B);
    perf.cycles_l12gdram = TransDataPerfEval(lh, rw, (GetOutput(0)->GetDataWidths()), TransferChannel::GDRAM_L1);
    perf.UpdateBottleneck();
    perf.PrintInfo(name_);
  }
  uint64_t BackwardEvalCycle() override {
    int flh = GetInputs()[0]->GetShape().GetExcludeLastDimCount();
    int flw = GetInputs()[0]->GetShape().GetLastDim();
    int frw = GetInputs()[1]->GetShape().GetDim(Axis::DIMW);
    // fwd: (lh, lw) & (lw, rw) -> (lh, rw)
    // bwd: intm (lh, rw) & (rw, lw) -> (lh, lw)
    //     weight(lw, lh & (lh, rw) -> (lw, rw)
    int bwlh = flw;
    int bwlw = flh;
    int bwrw = frw;
    MatmulEval(bwlh, bwlw, bwrw, dw_perf_);
    int bxlh = flh;
    int bxlw = frw;
    int bxrw = flw;
    MatmulEval(bxlh, bxlw, bxrw, dx_perf_);
    bwd_perf_ = dx_perf_ + dw_perf_;
    return bwd_perf_.GetBottleneck();
  }

  // workspace size, the operation needs extra DDR memory
  MatmulParam mm_param_;
  CycleCounter dw_perf_;
  CycleCounter dx_perf_;
};

}

#endif // NNTE_INCLUDE_OPS_MATMUL_H_

