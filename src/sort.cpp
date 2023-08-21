/***************************************************
#filename      : sort.cpp
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-15 10:56:21
#last modified : 2023-08-03 16:55:05
#description   : NA
***************************************************/
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iostream>
#include <map>
#include <functional>
#include <chrono>

#define BEGIN auto start = std::chrono::steady_clock::now();
#define END(str) auto end = std::chrono::steady_clock::now(); \
  std::cout << str << " sort "<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " ms." << std::endl;

using namespace std;

void is_sorted(std::string type, vector<int> &base, vector<int> sorted) {
  auto p = std::mismatch(base.begin(), base.end(), sorted.begin());
  if (p.first != base.end()) {
    cout << type << " sort error: value is not match, " << std::distance(base.begin(), p.first) << "(" << *p.first
         << ") != " << std::distance(sorted.begin(), p.second) << "("<< *p.second << ")" << endl;;
  }
  // cout << "error: datas[" << i << "]=" << datas[i] << " > datas[" << i+1 << "]=" << datas[i+1] << endl;
}

void pdata(const vector<int> &datas) {
  for_each(datas.begin(), datas.end(), [](const int &i){ cout << i << " " ;});
  cout <<endl;
}

vector<int> bubble_sort(vector<int> datas) {
  BEGIN;
  int n = datas.size();
  for (int i = 0; i < n ; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (datas[j] > datas[j+1]) {
        std::swap(datas[j], datas[j+1]);
      }
    }
  }
  END("bubble");
  return datas;
}

vector<int> select_sort(vector<int> datas){
  BEGIN;
  int n = datas.size();
  for (int i = 0; i < n ; i++) {
    int max = datas[0];
    int index = 0;
    int cn = n - i;
    for (int j = 0; j < cn; j++) {
      if (datas[j] > max) {
        index = j;
        max = datas[j];
      }
    }
    std::swap(datas[index], datas[cn-1]);
  }
  END("select");
  return datas;
}

vector<int> insert_sort(vector<int> datas) {
  BEGIN;
  int n = datas.size();
  for (int i = 1; i < n; i++) {
    int befor = i - 1;
    int v = datas[i];
    while (befor >= 0 && datas[befor] > v) {
      datas[befor+1] = datas[befor];
      befor--;
    }
    datas[befor+1] = v;
  }
  END("insert");
  return datas;
}


// merge sort
void merge_sub_sort(vector<int> & datas, int begin , int end) {
  if (begin == end || begin == end - 1) { // 最少拆解到一个数时返回，用于将此数与另一个数进行归并排序。
    return ;
  }
  int mid = (begin + end) / 2;
  merge_sub_sort(datas, begin, mid);
  merge_sub_sort(datas, mid, end);

  auto merge = [&] (vector<int> &datas, int begin, int mid, int end) {
    int fi = begin; // forward index. 前一个向量的索引。
    int bi = mid; // backward index. 后一个向量的索引。
    int i = 0;
    // 使用临时变量保存拍好序的结果，后续替换掉对应区间的未排序输入。用于递归中更高层的继续排序.
    vector<int> res(end-begin, 0);
    while (fi < mid && bi < end) {
      if (datas[fi] > datas[bi]) {
        res[i++] = datas[bi++];
      } else {
        res[i++] = datas[fi++];
      }
    }
    while (fi < mid) {
      res[i++] = datas[fi++];
    }
    while (bi < end) {
      res[i++] = datas[bi++];
    }
    std::copy(res.begin(), res.end(), std::next(datas.begin(), begin));
  };
  merge(datas, begin, mid, end);
}

vector<int> merge_sort(vector<int> datas) {
  BEGIN;
  int n = datas.size();
  merge_sub_sort(datas, 0, n);
  END("merge");
  return datas;
}

// quick sort
int quick_partition(vector<int> &datas, int begin, int end) {
  std::swap(datas[begin], datas[rand() % (end - begin) + begin]);
  int fi = begin; // forward index
  int bi = end; // backward index
  int mid_val = datas[begin];
  while (fi < bi) {
    // 1.1 先找到最后面的小于mid_value的数，及其位置。
    while (fi < bi && datas[bi] >= mid_val) { // 前面保证不会越级，后面保证递减. = 表明相等则不需变换
      bi--;
    }
    datas[fi] = datas[bi];

    // 1.2 将后面的小于mid_value的数:
    //      写到前面mid_index的位置。
    //      将mid_index改为后面小值对应的位置。
    // datas[mid_index] = datas[bi];
    // datas[bi] = mid_val;
    // mid_index = bi;

    // 2.1 找到前面的第一个大于mid_value的数，及其位置。
    while (fi < bi && datas[fi] <= mid_val) {  // 前面保证不会越级，后面保证递增
      fi++;
    }
    datas[bi] = datas[fi];


    // 2.2 将前面的第一个大于mid_value的数：
    //      写到后面，即mid_index所在位置。
    //      将mid_index改为前面大值对应的位置。
    // datas[mid_index] = datas[fi];
    // datas[fi] = mid_val;
    // mid_index = fi;
  }
  datas[fi] = mid_val;
  return fi;
}

void quick_sort_recurrence(vector<int> &datas, int begin , int end) {
  if (end - begin < 2) return;  // 单个元素自然有序
  int mid = quick_partition(datas, begin, end - 1);  // 左闭右开。
  quick_sort_recurrence(datas, begin, mid); // 函数内部会减一
  quick_sort_recurrence(datas, mid + 1, end); // 仅需锚点右侧的数据进行排序， 因此需要+1
}

vector<int> quick_sort(vector<int> datas) {
  BEGIN;

  quick_sort_recurrence(datas, 0, datas.size());

  END("quick");
  return datas;
}

/*
int t_partition(vector<int> &inputs, int b, int e) {
    if (b == e) {
        return b;
    }
    int mi = rand() % (e - b) + b;
    int mv = inputs[mi];
    swap(inputs[b], inputs[mi]);
    int bi = b;
    int ei = e;
    while (bi < ei) {
        while (bi < ei && inputs[ei] >= mv) {
            ei--;
        }
        inputs[bi] = inputs[ei];
        while (bi < ei && inputs[bi] < mv) {
            bi++;
        }
        inputs[ei] = inputs[bi];
    };
    inputs[bi] = mv;
    return bi;
}

void t_quick_sort(vector<int> &inputs, int b, int e) {
    if (b + 1 == e) {
        return;
    }
    // is-error. true:
    int p = t_partition(inputs, b, e);
    // true:
    int p = t_partition(inputs, b, e - 1);
    t_quick_sort(inputs, b, p);
    t_quick_sort(inputs, p+1, e);
}
*/

vector<int> radix_sort(vector<int> datas) {
  BEGIN;

  // quick_sort_recurrence(datas, 0, datas.size());

  END("radix");
  return datas;
}

vector<int> stl_sort(vector<int> datas) {
  std::sort(datas.begin(), datas.end(), std::less<int>());
  return datas;
}


std::map<string, std::function<vector<int>(vector<int>)>> str2sort = {
  {"BUBBLE", bubble_sort},
  {"SELECT", select_sort},
  {"INSERT", insert_sort},
  {"MERGE", merge_sort},
  {"QUICK", quick_sort},
  {"RADIX", radix_sort},
  {"STL", stl_sort},
};

void gen_data(vector<int>& datas, int nums) {
  random_device rd;
  mt19937 eng(rd());
  uniform_int_distribution<int> distr(1, 100); // INT32_MAX);
  // normal_distribution<> distr(1,100);
  for (int i = 0; i < nums; i++) {
    datas.push_back(distr(eng));
  }
  if (nums < 100) {
    pdata(datas);
  }
}


int main(int argn , char* argv[]) {
  if (argn != 2) {
    std::cout << "sort algrithm need data numbers as arg[1]" << std::endl;
    return 0;
  }
  int nums = atoi(argv[1]);
  // std::string sort_type(argv[2]);
  // std::transform(sort_type.begin(), sort_type.end(), sort_type.begin(), ::toupper);

  // 1. gen datas
  // vector<int> datas = {31, 7, 97, 36, 100, 66, 14, 57, 52, 93};
  vector<int> datas = {1, 20, 15, 30};
  gen_data(datas, nums);


  // 2.1 sort baseline
  vector<int> stl_sorted = str2sort["STL"](datas);

  // 2.2 speical sort algrithm
  is_sorted("QUICK", stl_sorted, str2sort["QUICK"](datas));
  is_sorted("MERGE", stl_sorted, str2sort["MERGE"](datas));
  is_sorted("INSERT", stl_sorted, str2sort["INSERT"](datas));
  is_sorted("SELECT", stl_sorted, str2sort["SELECT"](datas));
  is_sorted("BUBBLE", stl_sorted, str2sort["BUBBLE"](datas));
  return 0;
};

