/***************************************************
#filename      : embedding.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-01 14:24:58
#last modified : 2023-06-08 11:32:54
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
      : BaseLayer({in, table}, name) {
    Dims d = in->GetShape().GetDims();
    d.push_back(table->GetShape().GetDim(1));
    Shape s = {d, Layout::LNONE};
    auto out = Tensor::Create(s, table->GetDtype(), OpOutName(name));
    PushOutput(out);
  }

  void Check() override {}

  uint64_t ForwardEvalCycle() override {
    int token_num = GetInput(0)->GetShape().GetCount();
    int embedding_size = GetInput(1)->GetShape().GetDim(1);
    int voc_size = GetInput(1)->GetShape().GetCount();
    int voc_db = SizeOf(GetInput(1)->GetDtype());
    perf_.cycles_gdram2l1 = TransDataPerfEval(1, voc_size, voc_db, TransferChannel::GDRAM_L1);
    perf_.cycles_l12gdram = TransDataPerfEval(token_num, embedding_size, voc_db, TransferChannel::GDRAM_L1);
    perf_.UpdateBottleneck();
    perf_.PrintInfo(name_);
    return perf_.GetBottleneck();
  }
  uint64_t BackwardEvalCycle() override {
    return 0;
  }

};

}  // namespace nnte
#endif // NNTE_INCLUDE_OPS_EMBEDDING_H_
