#include "./include/mathFunctions.hpp"

int main(int argc, char *argv[]) {
  if (argc < 3){
      printf("Usage: %s base exponent \n", argv[0]);
      return 1;
  }
  double base = atof(argv[1]);
  int exponent = atoi(argv[2]);
  double result = power(base, exponent);

  // "\033[31m"
  //  \033 作为开头，代表一个ESC（固定字符）
  // 31是颜色代表（此为红色），可以多种属性组合，例如背景颜色或者字体颜色
  // m为此句结束
  // 0m代表关闭所有属性
  // 31红 32绿 33黄
  printf("\033[31m%g ^ %d is %g\n\033[0m", base, exponent, result);
  LOGI("litao1\n");
  LOGE("litao2\n");
  return 0;
}
