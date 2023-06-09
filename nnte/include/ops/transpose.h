/***************************************************
#filename      : transpose.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-01 16:28:12
#last modified : 2023-06-08 11:34:40
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_OPS_TRANSPOSE_U_
#define NNTE_INCLUDE_OPS_TRANSPOSE_U_

#include "../logger.h"
#include "../common.h"


namespace nnte {

class TransParam : public BaseParam {
 public:
  explicit TransParam(std::vector<int> &dst_order) : dst_order_(dst_order) {};
  explicit TransParam(std::vector<int> &&dst_order) : dst_order_(dst_order) {};
  void PrintInfo() override {
    std::stringstream infos;
    for (int i = 0; i < dst_order_.size(); i++) {
      PVSTR(infos, dst_order_[i]);
    }
  }

 public:
  std::vector<int> dst_order_;
};


class TransposeLayer : public BaseLayer {
 public:
  static Sptr<TransposeLayer> Create(Sptr<Tensor> in,
                                     TransParam param,
                                     std::string name = "TransposeLayer") {
    return std::make_shared<TransposeLayer>(in, param, name);
  }

  TransposeLayer(Sptr<Tensor> in, TransParam &param, std::string &name) :
    BaseLayer({in}, name), param_(param) {
    Check();
    auto s = in->GetShape();
    Dims dims = {};
    for (int i = 0; i < param.dst_order_.size(); i++) {
      dims.push_back(s.GetDim(param.dst_order_[i]));
    }
    auto out = Tensor::Create({dims, Layout::LNONE}, in->GetDtype());
    PushOutput(out);
  }

  uint64_t ForwardEvalCycle() override {
    perf_.PrintInfo(name_);
    return perf_.GetBottleneck();
  }
  uint64_t BackwardEvalCycle() override {
    return 0;
  }

  void Check() override {
    int in_dims_num = GetInputs()[0]->GetShape().GetDimNum();
    int p_dims_num = param_.dst_order_.size();
    if (in_dims_num != p_dims_num) {
      std::stringstream infos;
      infos << "transpese's input dimension is not equal transpose parameter's dst order:\n" ;
      PVSTR(infos, in_dims_num);
      PVSTR(infos, p_dims_num);
      LOG(E) << infos.str();
      throw "error!";
    }
  }
 private:
  TransParam param_;
};

}
#endif // NNTE_INCLUDE_OPS_TRANSPOSE_U_

