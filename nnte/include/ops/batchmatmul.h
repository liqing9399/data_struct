/***************************************************
#filename      : batchmatmul.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-05 17:25:15
#last modified : 2023-06-06 10:31:39
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_OPS_BATCHMATMUL_H_
#define NNTE_INCLUDE_OPS_BATCHMATMUL_H_

#include <utility>
#include "../logger.h"
#include "../common.h"
#include "../platform.h"

namespace nnte {

typedef enum MmOperandLoc {
  MM_LEFT = 0,
  MM_RIGHT,
} MmOperandLoc;

class BatchMatmulParam : public BaseParam {
 public:
  BatchMatmulParam(ActivationMode am, std::map<MmOperandLoc, std::pair<int,int>> matrix_index) : active_(am), mi_(matrix_index) {};
  BatchMatmulParam(const BatchMatmulParam &p) : active_(p.active_), mi_(p.mi_) {};
  BatchMatmulParam &operator=(const BatchMatmulParam &rhs) {
    this->active_ = rhs.active_;
    this->mi_ = rhs.mi_;
    return *this;
  }
  void PrintInfo() override {
    std::stringstream infos;
    std::string at = ActivationMode2String(active_);
    PVSTR(infos, at);
    PVSTR(infos, mi_[MM_LEFT].first);
    PVSTR(infos, mi_[MM_LEFT].second);
    PVSTR(infos, mi_[MM_RIGHT].first);
    PVSTR(infos, mi_[MM_RIGHT].second);
  }

 public:
  ActiveMode active_;
  std::map<MmOperandLoc, std::pair<int,int>> mi_;
};

class BatchMatmulLayer : public BaseLayer {
 public:
  static Sptr<BatchMatmulLayer> Create(Sptr<Tensor> left,
                                  Sptr<Tensor> right,
                                  Sptr<Tensor> bias,
                                  BatchMatmulParam param,
                                  std::string name = "BatchMatmulLayer") {
    return std::make_shared<BatchMatmulLayer>(left, right, bias, param, name);
  }

  BatchMatmulLayer(Sptr<Tensor> left,
              Sptr<Tensor> right,
              Sptr<Tensor> bias,
              BatchMatmulParam param,
              std::string &name)
      : BaseLayer({left, right}, {}, name), mm_param_(param) {
    if (bias) PushInput(bias);
    Check();

    int rw = GetInputs()[1]->GetShape().GetDim(mm_param_.mi_[MM_RIGHT].second);
    Dims d = left->GetShape().GetDims();
    d[mm_param_.mi_[MM_LEFT].second] =  rw;
    Shape s = {d, left->GetShape().GetLayout()};
    auto out = Tensor::Create(s, left->GetDtype(), OpOutName(name));
    PushOutput(out);
  }

  int GetLh() {
    int lh = GetInputs()[0]->GetShape().GetDim(mm_param_.mi_[MM_LEFT].first);
    return lh;
  }

  int GetLw() {
    int lw = GetInputs()[0]->GetShape().GetDim(mm_param_.mi_[MM_LEFT].second);
    return lw;
  }

  int GetRw() {
    int rw = GetInputs()[1]->GetShape().GetDim(mm_param_.mi_[MM_RIGHT].second);
    return rw;
  }

  int GetBatch() {
    int lh = GetLh();
    int lw = GetLw();
    int count = GetInputs()[0]->GetShape().GetCount();
    int l_batch = count / lh / lw;

    int rh = GetInputs()[1]->GetShape().GetDim(mm_param_.mi_[MM_RIGHT].first);
    int rw = GetRw();
    int r_count = GetInputs()[1]->GetShape().GetCount();
    int r_batch = r_count / rh / rw;
    if (l_batch != r_batch) {
      infos_ << name_ << ": input's batch is not equal.";
      PVSTR(infos_, l_batch);
      PVSTR(infos_, r_batch);
      throw name_ + "error!";
    }
    return l_batch;
  }

  void Check() override {
    int dims_i = GetInputs()[0]->GetShape().GetDimNum();
    int dims_w = GetInputs()[1]->GetShape().GetDimNum();
    // check shape of tensors is matched
    if (dims_i != dims_w ) {
      infos_ << "batch matmul need input's tensor dimension number is equal";
      PVSTR(infos_, dims_i);
      PVSTR(infos_, dims_w);
      throw  name_ + " error!";
    }
    int lh = GetLh();
    int lw = GetLw();
    int rh = GetInputs()[1]->GetShape().GetDim(mm_param_.mi_[MM_RIGHT].first);
    int rw = GetRw();

    int ln = GetInputs()[0]->GetShape().GetCount() / lh / lw;
    int rn = GetInputs()[1]->GetShape().GetCount() / rh / rw;
    bool is_matched = 1;
    infos_ << "matmul's operand shape not matched: ";
    if (lw != rh || ln != rn) {
      PVSTR(infos_, lw);
      PVSTR(infos_, rh);
      PVSTR(infos_, ln);
      PVSTR(infos_, rn);
      is_matched = 0;
    }

    if (GetInputs().size() == 3) {
      int bw = GetInputs()[2]->GetShape().GetDim(0);
      if (bw != rw) {
        PVSTR(infos_, rw);
        PVSTR(infos_, bw);
        is_matched = 0;
      }
    }
    if (!is_matched) {
      LOG(E) << infos_.str();
      throw name_ + " has error.";
    }
  }

  uint64_t EvaluateCycleV2() {
    int n = GetBatch();
    int lh = GetLh();
    int lw = GetLw();
    int rw = GetRw();
    perf_.cycles_mme = MmePerfEval(lh, lw, rw) * n;
    perf_.cycles_gdram2l1 = TransDataPerfEval(n * lh, lw, SizeOf(GetInputs()[0]->GetDtype()), TransferChannel::GDRAM_L1);
    perf_.cycles_gdram2llb = TransDataPerfEval(n * lw, rw, SizeOf(GetInputs()[1]->GetDtype()), TransferChannel::GDRAM_LLB);
    perf_.cycles_llb2l1_b = TransDataPerfEval(n * lw, rw, SizeOf(GetInputs()[1]->GetDtype()), TransferChannel::LLB_L1_B);
    perf_.cycles_l12gdram = TransDataPerfEval(n * lh, rw, SizeOf(GetOutputs()[0]->GetDtype()), TransferChannel::GDRAM_L1);
    perf_.UpdateBottleneck();
    perf_.PrintInfo(name_);
    return perf_.GetBottleneck();
  }

  uint64_t EvaluateCycle() override {
    uint64_t cycles = EvaluateCycleV2();
    return cycles;
  }


  // workspace size, the operation needs extra DDR memory
  BatchMatmulParam mm_param_;
};

}  // namespace nnte

#endif // NNTE_INCLUDE_OPS_BATCHMATMUL_H_

