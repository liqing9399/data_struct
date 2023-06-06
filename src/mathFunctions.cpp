/***************************************************
#filename      : mathFunctions.cpp
#author        : litao
#e-mail        : 362085095@qq.com
#create time   : 2022-09-23 11:12:29
#last modified : 2022-09-23 11:12:33
#description   : NA
***************************************************/
#include "../include/mathFunctions.hpp"



double power(double base, int exponent) {
  int result = base;
  int i;

  if (exponent == 0) {
    return 1;
  }

  for(i = 1; i < exponent; ++i){
    result = result * base;
  }
  return result;
}
