/***************************************************
#filename      : tree.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-19 11:24:00
#last modified : 2023-07-10 17:20:59
#description   : NA
***************************************************/
#ifndef NNTE_INCLUDE_TREE_H_
#define NNTE_INCLUDE_TREE_H_

#include <vector>
#include <functional>
#include <set>
#include <map>
#include <stack>
#include <cstdlib>
#include <iostream>
#include "./logger.h"

namespace ds {

typedef enum class VecType {
 IN_POST = 0,
 IN_PRE,
} VecType;

#define Stature(x) ((x) ? (x)->height : -1)

template <typename T>
struct BinNode {
  T data;
  int height; // 当前节点至底层叶子节点的最大值。
  BinNode<T> *parent;
  BinNode<T> *lf_s;
  BinNode<T> *rh_s;

  explicit BinNode(T v) : data(v), height(0), parent(nullptr), lf_s(nullptr), rh_s(nullptr) {}

  void InsertRn(BinNode<T> *rn) {
    rh_s = rn;
    UpdateHeight();
  };
  void InsertLn(BinNode<T> *ln) {
    lf_s = ln;
    UpdateHeight();
  };
  int UpdateHeight() {
    height = 1 + std::max(Stature(lf_s), Stature(lf_s));
    return height;
  };
};

typedef enum class TraverseType {
  T_PREORDER = 0,
  T_INORDER,
  T_POSTORDER,
  T_PREORDER_ITER,
  T_INORDER_ITER,
  T_POSTORDER_ITER,
} TraverseType;

template <typename T>
class BinTree {
 protected:
  int size_;
  BinNode<T> *root_;

 public:
  static BinTree *MakeTree() {
    BinTree *bt = new (BinTree);
    return bt;
  };

  static BinTree *MakeTree(VecType type, std::vector<T> &v1, std::vector<T> &v2) {
    BinTree *bt = new BinTree(type, v1, v2);
    return bt;
  }

  BinTree() : size_(0), root_(nullptr) {};

  BinTree(VecType type, std::vector<T> &v1, std::vector<T> &v2) : size_(v1.size()), root_(nullptr) {
    if (type == VecType::IN_POST) {
      root_ = InPostConstructor(v1, v2);
    } else {
      root_ = InPreConstructor(v1, v2);
    }
  };

  ~BinTree() {
    if (size_ >0)  {
      int nn = RemoveSubTree(root_);
      if (nn != size_) {
        LOG(I) << "remove node number is not equal tree's size. " << nn << size_;
      }
    }
  }

  int RemoveSubTree(BinNode<T> *root) {
    // 0. 为空则返回
    if (!root) {
      return 0;
    }
    // 1. 更新树的高度。
    UpdateHeightAbove(root->parent);

    // 2. 删除父节点中的关系。
    if (root->parent) {
      if (root->parent->lf_s == root) {
        root->parent->lf_s = nullptr;
      } else {
        root->parent->lf_r = nullptr;
      }
    }
    // 3. 删除子节点中的关系。  // 不需要。因为本节点的所有资源会被释放，因此是否置 nullptr 无关紧要。
    // 4. 释放内存。
    int ln = RemoveSubTree(root->lf_s);
    if (ln) size_--;

    int rn = RemoveSubTree(root->rf_s);
    if (rn) size_--;
    delete root;
    return ln + rn;
  }

  void UpdateNodeHeight(BinNode<T>* x) {
    x->height = 1 + std::max(Stature(x->lf_s), Stature(x->lf_s));
  }

  void UpdateHeightAbove(BinNode<T> *x) {
    while (x) {
      UpdateNodeHeight(x);
      x = x->parent;
    }
  }

  int Size() {return size_;};
  bool IsEmpty() {return size_;};
  void Draw() {
    auto f = [] (BinNode<T> * n) {
      std::cout << n->data << " ";
    };
    std::function<void(BinNode<T>*)> p = f;
    Travese(p);
  }

  // static std::map<std::string, std::function<void(void)>> target_travse = {
  //   {"Preorder", BinTree<T>::TravesePreorder},
  //   {"Postorder", BinTree<T>::TravesePostorder},
  // };

  void Travese(std::function<void(BinNode<T>*)> &f) {
    LOG(I) << "TravesePreorder:";
    TravesePreorder(root_, f);
    std::cout << "\n";
    LOG(I) << "TravesePreorderIter:";
    TravesePreorderIter(root_, f);
    std::cout << "\n";

    LOG(I) << "TraveseInorder:";
    TraveseInorder(root_, f);
    std::cout << "\n";
    LOG(I) << "TraveseInorderIter:";
    TraveseInorderIter(root_, f);
    std::cout << "\n";

    LOG(I) << "TravesePostorder:";
    TravesePostorder(root_, f);
    std::cout << "\n";
    LOG(I) << "TravesePostorderIter:";
    TravesePostorderIter(root_, f);
    std::cout << "\n";

  }

  std::vector<T> GetValue(TraverseType type) {
    std::vector<T> res;
    auto p = [&res] (BinNode<T> *nd) {
      if (nd) {
        res.push_back(nd->data);
      }
    };
    std::function<void(BinNode<T>*)> f = p;
    switch (type) {
      case TraverseType::T_PREORDER:
        TravesePreorder(root_, f); break;
      case TraverseType::T_INORDER:
        TraveseInorder(root_, f); break;
      case TraverseType::T_POSTORDER:
        TravesePostorder(root_, f); break;
      case TraverseType::T_PREORDER_ITER:
        TravesePreorderIter(root_, f); break;
      case TraverseType::T_INORDER_ITER:
        TraveseInorderIter(root_, f); break;
      case TraverseType::T_POSTORDER_ITER:
        TraveseInorderIter(root_, f); break;
      default:
        TraveseInorder(root_, f);
    }
    return res;
  }

  void TravesePreorder(BinNode<T> *root, std::function<void(BinNode<T>*)> &f) {
    if (root == nullptr) return;
    f(root);
    TravesePreorder(root->lf_s, f);
    TravesePreorder(root->rh_s, f);
  }

  void TraveseInorder(BinNode<T> *root, std::function<void(BinNode<T>*)> &f) {
    if (root == nullptr) return;
    TraveseInorder(root->lf_s, f);
    f(root);
    TraveseInorder(root->rh_s, f);
  }

  void TravesePostorder(BinNode<T> *root, std::function<void(BinNode<T>*)> &f) {
    if (root == nullptr) return;
    TravesePostorder(root->lf_s, f);
    TravesePostorder(root->rh_s, f);
    f(root);
  }

  void TravesePreorderIter(BinNode<T> *root, std::function<void(BinNode<T>*)> &f) {
    std::stack<BinNode<T>*> right_tree;
    while (root) {
      f(root);
      if (root->rh_s) {
        right_tree.push(root->rh_s);
      }
      if (root->lf_s) {
        root = root->lf_s;
      } else {
        if (right_tree.empty()) {
          root = nullptr;
        } else {
          root = right_tree.top();
          right_tree.pop();
        }
      }
    }
  }

  void TraveseInorderIter(BinNode<T> *root, std::function<void(BinNode<T>*)> &f) {
    // 1. 先搜素到最左边节点的空节点, 并将最路径上的节点均放入栈中，以备后续访问。此时所有非空节点均放置在栈中。
    // 2. 从非空栈中取出最后一个节点。
    // 3. 访问此节点。
    // 4. 从右节点重新开始上述循环。直到处理完此右子树。（如果不存在右节点，则此节点为叶子节点.
    std::stack<BinNode<T>*> in_and_right_tree;
    do {
      if (root) {
        in_and_right_tree.push(root);
        root = root->lf_s;
      } else if (in_and_right_tree.size()) {
        auto mid = in_and_right_tree.top(); in_and_right_tree.pop();
        f(mid);
        root = mid->rh_s;
      }
    } while (in_and_right_tree.size() || root);
  }

  void TravesePostorderIter(BinNode<T> *root, std::function<void(BinNode<T>*)> &f) {
    std::stack<BinNode<T>*> s;
    s.push(root);
    while (!s.empty()) {
      if (s.top() != root->parent) {
        while (s.top()) {
          auto last_n = s.top();
          if (last_n->lf_s) {
            if (last_n->rh_s) {
              s.push(last_n->rh_s);
            }
            s.push(last_n->lf_s);
          } else {
            s.push(last_n->rh_s);
          }
        }
      }
      s.pop();
      root = s.top(); s.pop();
      f(root);
    }
  }

 private:
  void CheckSizeAndValue(std::vector<T> &v, std::vector<T> &v2) {
    if (v.size() != v2.size()) {
      LOG(E) << "BinTree can not be constructed by vectors has not eqaul element numbers: v1.size() = " << v.size()
             << ", v2.size() = " << v2.size();
      throw "error";
    }
    std::set<T> s;
    for (auto i : v) {
      if (s.find(i) != s.end()) {
        LOG(E) << "BinTree can not be constructed by vector has repeated number. repeated number is: " << i;
        throw "error";
      }
    }
  }

  BinNode<T>* InPreConstructor(std::vector<T> &inorder, std::vector<T> &preorder) {
    if (inorder.size() == 0) return nullptr;
    CheckSizeAndValue(inorder, preorder);
    int n = inorder.size();
    T root_v = preorder[0];
    int root_index = 0;
    for (; root_index < n ; root_index++) {
      if (inorder[root_index] == root_v) {
        break;
      }
    }

    BinNode<T> *nd = new BinNode<T>(root_v);

    std::vector<T> inorder_left(inorder.begin(), inorder.begin() + root_index);
    std::vector<T> inorder_right(inorder.begin() + root_index + 1, inorder.end());

    std::vector<T> preorder_left(preorder.begin() + 1, preorder.begin() + inorder_left.size() + 1);
    std::vector<T> preorder_right(preorder.begin() + 1 + preorder_left.size(), preorder.end());

    auto lf = InPreConstructor(inorder_left, preorder_left);
    auto rh = InPreConstructor(inorder_right, preorder_right);
    nd->InsertRn(rh);
    nd->InsertLn(lf);

    return nd;

  }

  BinNode<T>* InPostConstructor(std::vector<T> &inorder, std::vector<T> &postorder) {
    if (inorder.empty()) return nullptr;
    CheckSizeAndValue(inorder, postorder);

    int n = postorder.size();
    T root_v = postorder[n - 1];

    BinNode<T> *bn = new BinNode<T>(root_v);
    int ri_inorder = 0;
    for (int i = 0; i < n; i++) {
      if (inorder[i] == root_v) {
        ri_inorder = i;
        break;
      }
    }
    std::vector<T> in_left(inorder.begin(), inorder.begin() + ri_inorder);
    std::vector<T> in_right(inorder.begin() + ri_inorder + 1, inorder.end());

    std::vector<T> post_left(postorder.begin(), postorder.begin() + in_left.size());
    std::vector<T> post_right(postorder.begin() + in_left.size(), postorder.end() - 1);

    auto ln = InPostConstructor(in_left, post_left);
    auto rn = InPostConstructor(in_right, post_right);
    bn->InsertRn(rn);
    bn->InsertLn(ln);

    return bn;
  }
};

#endif // NNTE_INCLUDE_TREE_H_

} // namespace ds
