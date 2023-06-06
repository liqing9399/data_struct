/***************************************************
#filename      : embedding.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-01 14:24:58
#last modified : 2023-06-05 16:18:02
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_OPS_EMBEDDING_H_
#define NNTE_INCLUDE_OPS_EMBEDDING_H_

#include "../logger.h"
#include "../common.h"

namespace nnte {
class EmbeddingLayer : public BaseLayer {
 public:
  static Sptr<EmbeddingLayer> Create(Sptr<Tensor> in,
                                     Sptr<Tensor> table,
                                     std::string name = "EmbeddingLayer") {
    return std::make_shared<EmbeddingLayer>(in, table, name);
  }

  EmbeddingLayer(Sptr<Tensor> in,
                 Sptr<Tensor> table,
                 std::string &name)
      : BaseLayer({in, table}, {}, name) {
    Dims d = in->GetShape().GetDims();
    d[d.size() - 1] =  table->GetShape().GetDim(Axis::DIMW);
    Shape s = {d, in->GetShape().GetLayout()};
    auto out = Tensor::Create(s, in->GetDtype(), OpOutName(name));
    PushOutput(out);
  }

  void Check() override {}

  uint64_t EvaluateCycle() override {
    return 0;
  }
};

}  // namespace nnte
#endif // NNTE_INCLUDE_OPS_EMBEDDING_H_
