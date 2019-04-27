
#include <iostream>
#define addddd(T, t) (t+T)
#define add(x,...) addddd(x,##__VA_ARGS__)
#define YEAR(year) (year * 24) // hour


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
  return 0;
}
