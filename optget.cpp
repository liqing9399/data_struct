#include <unistd.h>
#include <stdio.h>
#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
  int ch = 0;
  int version = 10;
  std::string name = "";
  printf("litao argument test\n");

  // "a:b:cd::e"，这就是一个选项字符串。对应到命令行就是-a ,-b ,-c ,-d, -e 。
  // 冒号又是什么呢？ 冒号表示参数，一个冒号就表示这个选项后面必须带有参数（没有带参数会报错哦）
  // 但是这个参数可以和选项连在一起写，也可以用空格隔开，比如-a123 和-a   123（中间有空格） 都表示123是-a的参数.
  // 两个冒号的就表示这个选项的参数是可选的，即可以有参数，也可以没有参数没有参数
  // 但要注意有参数时，参数与选项之间不能有空格（有空格会报错的哦），这一点和一个冒号时是有区别的。
  // 好了，先给个代码，然后再解释吧。
  while ((ch = getopt(argc, argv, "ab::v:n:")) != -1) {
    printf("optind: %d\n", optind);
    switch(ch) {
      case 'a':
        std::cout << "argment is a" << std::endl;
        break;
      case 'b':
        std::cout << "argment is b" << std::endl;
        std::cout << "optarg is " <<  optarg << std::endl;
        break;
      case 'n':
        name = "ruirui";
        std::cout << "argment is n" << std::endl;
        std::cout << "optarg is " <<  optarg << std::endl;
        break;
      case 'v':
        version = 20;
        std::cout << "argment is v" << std::endl;
        std::cout << "optarg is " <<  optarg << std::endl;
        break;
      default:
        std::cout << "unsupport argment" << std::endl;
        break;
    }
  }
  std::cout << "ch: " << ch
            << "version: " << version
            << "name: "  << name
            << std::endl;
}
