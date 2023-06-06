/***************************************************
#filename      : utils.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-26 15:07:23
#last modified : 2023-06-05 16:43:12
#description   : NA
***************************************************/

#ifndef NNTE_INCLUDE_UTILS_H_
#define NNTE_INCLUDE_UTILS_H_

#include <memory>
#include <vector>

namespace nnte {

template <typename T>
using Sptr = std::shared_ptr<T>;

template <typename T>
using vSptr = std::vector<Sptr<T>>;


template <typename T>
inline void FreePtr(T **ptr) {
  if (*ptr) {
    free(*ptr);
    *ptr = nullptr;
  }
}

int inline ceil_2x(int x, int base) {
  int r = (((x) + base - 1) & ~(base - 1));
  return r;
}

std::string OpOutName(std::string &op_name, int index = 0) {
  return op_name + "_out_" + std::to_string(index);
}

// 定义可以处理变长参数的宏
// #define DEF_ENUM(enum_name, num, ...)

}

#endif // NNTE_INCLUDE_UTILS_H_
