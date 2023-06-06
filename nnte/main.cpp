/***************************************************
#filename      : main.cpp
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-26 11:44:36
#last modified : 2023-06-05 20:05:59
#description   : NA
***************************************************/

// #include "./include/common.h"
#include "./include/ops.h"

using namespace nnte;

void ConstructTransfromerBlock(int tbsi,
															 int head_num,
															 Sptr<Network> network,
                               Sptr<Tensor> ti,
                               vSptr<Tensor> atte_w, vSptr<Tensor> ffn_w1, vSptr<Tensor> ffn_w2,
                               vSptr<Tensor> atte_b, vSptr<Tensor> ffn_b1, vSptr<Tensor> ffn_b2) {
  Sptr<Network> transformer_block = Network::Create("transformer_block_" + std::to_string(tbsi));
  auto GetWIndex = [&] (int wi) -> int {
    int i = tbsi * 4 + wi;
    return i;
  };
  auto layernorm_1 = LayerNormLayer::Create(ti);
  auto mm_q = MatmulLayer::Create(layernorm_1->GetOutputs()[0], atte_w[GetWIndex(0)], atte_b[GetWIndex(0)],
                MatmulParam(), "mm_q");
  auto mm_k = MatmulLayer::Create(mm_q->GetOutputs()[0], atte_w[GetWIndex(1)],
                atte_b[GetWIndex(1)], MatmulParam(), "mm_k");
  auto mm_v = MatmulLayer::Create(mm_q->GetOutputs()[0], atte_w[GetWIndex(2)],
                atte_b[GetWIndex(2)], MatmulParam(), "mm_v");

  int seqlen = ti->GetShape().GetDim(Axis::DIMH);
  int query_size = ti->GetShape().GetDim(Axis::DIMW) / head_num;

  // (seqlen, hidden) -> (seqlen, head_num, query_size)
  auto rs_q = ReshapeLayer::Create(mm_q->GetOutputs()[0], {seqlen, head_num, query_size}, "q_reshape");
  // (seqlen, hidden) -> (hidden, seqlen)
  auto tr_k = TransposeLayer::Create(mm_k->GetOutputs()[0], TransParam({1, 0}), "ts_k");
  // (hidden, seqlen) -> (head_num, query_size, seqlen)
  auto rs_k = ReshapeLayer::Create(tr_k->GetOutputs()[0], {head_num, query_size, seqlen}, "k_reshape");
  // (seqlen, hidden) -> (seqlen, head_num, query_size)
  auto rs_v = ReshapeLayer::Create(mm_v->GetOutputs()[0], {seqlen, head_num, query_size}, "v_reshape");

  BatchMatmulParam param_qk = {ActivationMode::AM_NONE, {{MM_LEFT, {0, 2}},{MM_RIGHT, {1, 2}}} };
  auto bmm_qk = BatchMatmulLayer::Create(rs_q->GetOutputs()[0], rs_k->GetOutputs()[0],
                  nullptr, param_qk, "bmm1");
  // auto softmax = ...;
  BatchMatmulParam param_kv = {ActivationMode::AM_NONE, {{MM_LEFT, {0, 2}},{MM_RIGHT, {0, 2}}} };
  auto bmm_kv = BatchMatmulLayer::Create(bmm_qk->GetOutputs()[0], rs_v->GetOutputs()[0],
                  nullptr, param_kv, "bmm2");
  auto rs_kv_r = ReshapeLayer::Create(bmm_kv->GetOutputs()[0], {seqlen, head_num * query_size}, "bmm_kv_reshape");
  auto mm_3 = MatmulLayer::Create(rs_kv_r->GetOutputs()[0], atte_w[GetWIndex(3)],
                  atte_b[GetWIndex(3)], MatmulParam(), "mm3");
  auto add_1 = BinaryLayer::Create(mm_3->GetOutputs()[0], ti);  // add

  auto layernorm_2 = LayerNormLayer::Create(add_1->GetOutputs()[0]);

  auto mm_ffn_1 = MatmulLayer::Create(layernorm_2->GetOutputs()[0], ffn_w1[GetWIndex(0)],
                  ffn_b1[GetWIndex(0)], MatmulParam(ActivationMode::AM_GELU), "ffn_1");
  auto mm_ffn_2 = MatmulLayer::Create(mm_ffn_1->GetOutputs()[0], ffn_w2[GetWIndex(0)],
                  ffn_b2[GetWIndex(0)], MatmulParam(), "ffn_2");
  auto add_2 = BinaryLayer::Create(mm_ffn_2->GetOutputs()[0], layernorm_2->GetOutputs()[0]);  // add
  transformer_block->AddOperator(layernorm_1);
  transformer_block->AddOperator(mm_q);
  transformer_block->AddOperator(mm_k);
  transformer_block->AddOperator(mm_v);
  transformer_block->AddOperator(rs_q);
  transformer_block->AddOperator(tr_k);
  transformer_block->AddOperator(rs_k);
  transformer_block->AddOperator(rs_v);
  transformer_block->AddOperator(bmm_qk);
  transformer_block->AddOperator(bmm_kv);
  transformer_block->AddOperator(mm_3);
  transformer_block->AddOperator(add_1);
  transformer_block->AddOperator(layernorm_2);
  transformer_block->AddOperator(mm_ffn_1);
  transformer_block->AddOperator(mm_ffn_2);
  auto block_cycles = transformer_block->EvaluateCycle();
  LOG(I) << "single transformer block cycles: " << block_cycles;
  network->AddOperator(transformer_block);
}

int main() {
  int seqlen = 2048;
  int voc_size = 145664;
  int embedding_size = 12288;
  int gelu_w = embedding_size * 4;
  int transformer_block_num = 1;  // layer number
  int head_num = 96;

  nnte::Dims di = {seqlen, embedding_size};
  nnte::Dims dembt = {voc_size, embedding_size};
  nnte::Dims dw1 = {embedding_size, embedding_size};
  nnte::Dims dw2 = {embedding_size, gelu_w};
  nnte::Dims dw3 = {gelu_w, embedding_size};

  nnte::Shape si(di, Layout::HW);
  nnte::Shape sembt(dembt, Layout::HW);
  nnte::Shape sw1(dw1, Layout::HW);
  nnte::Shape sw2(dw2, Layout::HW);
  nnte::Shape sw3(dw3, Layout::HW);

  nnte::Shape sb1({embedding_size}, Layout::LNONE);
  nnte::Shape sb2({gelu_w}, Layout::LNONE);
  nnte::Shape sb3({embedding_size}, Layout::LNONE);

  // input seqence
  nnte::Sptr<nnte::Tensor> ti = nnte::Tensor::Create(si, nnte::Datatype::FLOAT16, "input");

  // weight parameter.
  nnte::Sptr<nnte::Tensor> tembt = nnte::Tensor::Create(sembt, nnte::Datatype::FLOAT16, "embedding_table");
  nnte::vSptr<nnte::Tensor> tw1 = nnte::Tensor::CreateVec(transformer_block_num * 4, sw1, nnte::Datatype::FLOAT16, "atte_w");
  nnte::vSptr<nnte::Tensor> tw2 = nnte::Tensor::CreateVec(transformer_block_num, sw2, nnte::Datatype::FLOAT16, "ffn_w1");
  nnte::vSptr<nnte::Tensor> tw3 = nnte::Tensor::CreateVec(transformer_block_num, sw3, nnte::Datatype::FLOAT16, "ffn_w2");
  nnte::vSptr<nnte::Tensor> b1 = nnte::Tensor::CreateVec(transformer_block_num * 4, sb1, nnte::Datatype::FLOAT16, "atte_b");
  nnte::vSptr<nnte::Tensor> b2 = nnte::Tensor::CreateVec(transformer_block_num, sb2, nnte::Datatype::FLOAT16, "ffn_b1");
  nnte::vSptr<nnte::Tensor> b3 = nnte::Tensor::CreateVec(transformer_block_num, sb3, nnte::Datatype::FLOAT16, "ffn_b2");

  Sptr<Network> net = Network::Create();
  auto embedding = EmbeddingLayer::Create(ti, tembt);
  net->AddOperator(embedding);
  // for (int i = 0; i < transformer_block_num; i++) {
	// 	ConstructTransfromerBlock(i, head_num, net, embedding->GetOutputs()[0], tw1, tw2, tw3, b1, b2, b3);
  // }
	ConstructTransfromerBlock(0, head_num, net, embedding->GetOutputs()[0], tw1, tw2, tw3, b1, b2, b3);
  auto cycles = net->EvaluateCycle();
  std::cout << "net's cycle time:" << cycles << std::endl;
  return 0;
}
