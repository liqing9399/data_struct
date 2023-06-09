/***************************************************
#filename      : binary.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-08 11:37:29
#last modified : 2023-06-08 14:43:06
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_OPS_BINARY_H_
#define NNTE_INCLUDE_OPS_BINARY_H_
#include "../logger.h"
#include "../common.h"

namespace nnte {
enum class BinaryType {
  ADD,
  SUB,
  MUL,
};

class BinaryLayer : public BaseLayer {
 public:
  static Sptr<BinaryLayer> Create(Sptr<Tensor> left,
                                  Sptr<Tensor> right,
                                  std::string name = "BinaryLayer") {
    return std::make_shared<BinaryLayer>(left, right, name);
  }

  BinaryLayer(Sptr<Tensor> left, Sptr<Tensor> right, std::string &name) :
    BaseLayer({left, right}, name) {
    Check();
    auto s = GetOutShape(left, right);
    auto out = Tensor::Create(s, left->GetDtype());
    PushOutput(out);
  }

  uint64_t ForwardEvalCycle() override {
    auto ls = GetInputs()[0]->GetShape();
    auto rs = GetInputs()[1]->GetShape();

    int dn = ls.GetDimNum();
    int m = 1;
    for (int i = 0; i < dn - 1; ++i) {
      m *= std::max(ls.GetDim(i), rs.GetDim(i));
    }
    int n = std::max(ls.GetDim(dn - 1), rs.GetDim(dn - 1));
    // int t = 3;  // match for hardware
    // int k = 1;
    // uint64_t cycles = 10 + m * (n + VME - 1) / VME * t * k;
    perf_.cycles_vme = VmePerfEval(m, n, m, n);
    perf_.cycles_gdram2l1 = DmaPerfEval(m, n, SizeOf(GetInputs()[0]->GetDtype())) * 2;
    perf_.cycles_l12gdram = DmaPerfEval(m, n, SizeOf(GetOutputs()[0]->GetDtype()));
    perf_.UpdateBottleneck();
    perf_.PrintInfo(name_);
    return perf_.GetBottleneck();
  }
  uint64_t BackwardEvalCycle() override {
    return 0;
  }

  void Check() override {
    auto ls = GetInputs()[0]->GetShape();
    auto rs = GetInputs()[1]->GetShape();
    int is_valid = ls.GetDimNum() == rs.GetDimNum();
    if (!is_valid) {
      LOG(E) << "error: inputs shape's dimension number is not equal."
             << "in1.dim_num = " << ls.GetDimNum()
             << "in2.dim_num = " << rs.GetDimNum();
    }
  }
 private:
  Shape GetOutShape(Sptr<Tensor> left, Sptr<Tensor> right) {
    auto ls = left->GetShape();
    auto rs = right->GetShape();
    int nums = ls.GetDimNum();
    assert(nums == rs.GetDimNum());
    Dims dims(nums);
    for (int i = 0; i < ls.GetDimNum(); ++i) {
      dims[i] = ls.GetDim(i) > rs.GetDim(i) ? ls.GetDim(i) : rs.GetDim(i);
    }
    return Shape(dims, rs.GetLayout());
  }
  // BinaryType mode_;
};

}

#endif // NNTE_INCLUDE_OPS_BINARY_H_

