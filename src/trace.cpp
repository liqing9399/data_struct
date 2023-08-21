/***************************************************
#filename      : trace.cpp
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-07-12 17:50:38
#last modified : 2023-07-13 10:23:22
#description   : NA
***************************************************/
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

int main(int argc,char *argv[]) {
  vector<int> result;
  int dst_len = 100;
  auto trace = [&] (vector<int> path, vector<int> choices) {
    if (path.size() == dst_len) {
      result = path;
    }
    for (auto v : choices) {
      if (std::find(path.begin(), path.end(), v) != v.end()) {
        continue;
      }
      trace(path, choices);
      path.pop();
    }

  };
  trace(path, inputs);
  return 0;
}



