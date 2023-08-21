/***************************************************
#filename      : main.cpp
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-14 17:02:42
#last modified : 2023-06-20 15:21:51
#description   : NA
***************************************************/
#include "./include/mathFunctions.h"
#include "./include/tree.h"

using namespace ds;

void compare(std::vector<int> &v1, std::vector<int> &v2, std::string name) {
  if (std::mismatch(v1.begin(), v1.end(), v2.begin()).first != v1.end()) {
    LOG(E) << "ERROR:" << name;
  } else {
    LOG(I) << "true:" << name;
  };
}

void ConstructBinTreeTest(std::vector<int> &in, std::vector<int> &post) {
  BinTree<int> *in_post = ds::BinTree<int>::MakeTree(VecType::IN_POST, in, post);
  in_post->Draw();
  auto in_post_pre = in_post->GetValue(TraverseType::T_PREORDER);
  auto in_post_in = in_post->GetValue(TraverseType::T_INORDER);
  auto in_post_post = in_post->GetValue(TraverseType::T_POSTORDER);

  auto in_post_pre_iter = in_post->GetValue(TraverseType::T_PREORDER_ITER);
  auto in_post_in_iter = in_post->GetValue(TraverseType::T_INORDER_ITER);
  auto in_post_post_iter = in_post->GetValue(TraverseType::T_POSTORDER_ITER);
  compare(in_post_pre, in_post_pre_iter, "preorder_iter");
  compare(in_post_in, in_post_in_iter,  "inorder_iter");
  compare(in_post_post,in_post_post_iter, "postorder_iter");

  BinTree<int> *in_pre = ds::BinTree<int>::MakeTree(VecType::IN_PRE, in_post_in, in_post_pre);
  auto in_pre_pre = in_pre->GetValue(TraverseType::T_PREORDER);
  auto in_pre_in = in_pre->GetValue(TraverseType::T_INORDER);
  auto in_pre_post = in_pre->GetValue(TraverseType::T_POSTORDER);

  auto in_pre_pre_iter = in_pre->GetValue(TraverseType::T_PREORDER_ITER);
  auto in_pre_in_iter = in_pre->GetValue(TraverseType::T_INORDER_ITER);
  auto in_pre_post_iter = in_pre->GetValue(TraverseType::T_POSTORDER_ITER);
  compare(in_pre_pre, in_pre_pre_iter, "preorder_iter");
  compare(in_pre_in, in_pre_in_iter, "inorder_iter");
  compare(in_pre_post, in_pre_post_iter, "postorder_iter");

  compare(in_post_pre, in_pre_pre, "preorder");
  compare(in_post_in, in_pre_in, "inorder");
  compare(in_post_post, in_pre_post, "postorder");
  in_pre->Draw();
  LOG(I) << "BinTree is empty: " << in_pre->IsEmpty();
}

int main(int argc, char *argv[]) {
  std::vector<int> in = {9,3,15,20,7};
  std::vector<int> post = {9,15,7,20,3};
  ConstructBinTreeTest(in, post);
  std::vector<int> pre1 = {1, 2, 3, 6, 4, 5, 7, 8, 9, 11, 10};
  std::vector<int> in1 = {3, 2, 4, 6, 5, 1, 8, 9, 7, 10, 11};
  std::vector<int> post1 = {3, 4, 5, 6, 2, 9, 8, 10, 11, 7, 1};
  ConstructBinTreeTest(in1, post1);
  // ConstructBinTreeTest(in1, pre1);

  return 0;
}
