
#include <iostream>
#define addddd(T, t) (t+T)
#define add(x,...) addddd(x,##__VA_ARGS__)
#define YEAR(year) (year * 24) // hour


#define MAX(a, b) ((a)>(b)?(a):(b))

int a[] = {1, 3, 5};
int max(int n) {
  return n == 0 ? a[0] : MAX(a[n], max(n-1));
}

int main() {
  std::cout << "begin" << std::endl;
  int i = 0;
  i = YEAR(2);
  std::cout << "year:" << i << std::endl;
  std::cout << "year:" << i << std::endl;
  std::cout << "year:" << i << std::endl;
  std::cout << "year:" << i << std::endl;
  i = add(i, 1);
  // i = add(i);
  std::cout << "year " << i << std::endl;


  // int a[] = {9, 3, 5, 2, 1, 0, 8, 7, 6, 4 };
  // int max_value = max(9);
  int max_value = max(3);

  std::cout << "max_value:" << max_value << std::endl;

  return 0;
}
