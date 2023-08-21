/***************************************************
#filename      : mathFunctions.cpp
#author        : litao
#e-mail        : 362085095@qq.com
#create time   : 2022-09-23 11:12:29
#last modified : 2023-06-19 11:23:24
#description   : NA
***************************************************/
#include "../include/mathFunctions.h"



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
