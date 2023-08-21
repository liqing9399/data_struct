/***************************************************
#filename      : binaraySearch.cpp
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-07-12 17:17:57
#last modified : 2023-07-12 17:24:05
#description   : NA
***************************************************/
#include <iostream>
#include <vector>
#include <string>

using namespace std;

bool isTrue(int m) {
  return m;
}
int main(int argc,char *argv[]) {
  int N = 100;
  int l = -1;
  int r = N;
  while (l + 1 != r) {
    int m = (l+r)/2;
    if (isTrue(m)) {
      l = m;
    } else {
      r = m;
    }
  }
  return l;
}



