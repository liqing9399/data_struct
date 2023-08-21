/***************************************************
#filename      : algrithm.cpp
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-15 14:38:22
#last modified : 2023-06-15 15:29:56
#description   : NA
***************************************************/
#include <vector>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <functional>

int main()
{
    std::vector<int> v(10, 2);
    // for_each 遍历每个元素，进行f中的操作。
    std::cout << "values: ";
    for_each(v.begin(), v.end(), [] (int i) { std::cout << i << " ";});
    std::cout << '\n';

    // partial_sum 输出的每个元素为对应的前面所有元素的和
    // xx.cbegin() 返回的是常量迭代器
    std::partial_sum(v.cbegin(), v.cend(), v.begin());
    for_each(v.cbegin(), v.cend(), [] (int i) { std::cout << i << " ";});
    std::cout << '\n';

    std::cout << "Among the numbers: ";
    // copy 将源数据逐个拷贝到目的迭代器。
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << '\n';

    // all_of 返回bool值, 当所有元素均返回true时，返回true，否则返回false。
    if (std::all_of(v.cbegin(), v.cend(), [](int i){ return i % 2 == 0; })) {
        std::cout << "All numbers are even\n";
    }

    // none_of 返回bool值，当所有元素均返回false时，返回true，否则返回false.
    if (std::none_of(v.cbegin(), v.cend(), std::bind(std::modulus<>(),
                                                     std::placeholders::_1, 2))) {
        auto res = std::find_if(v.cbegin(), v.cend(), std::bind(std::modulus<>(), std::placeholders::_1, 2));
        std::cout << "None of them are odd. last find:" << std::distance(v.cbegin(), res) << std::endl;
    }
    struct DivisibleBy
    {
        const int d;
        explicit DivisibleBy(int n) : d(n) {}
        bool operator()(int n) const { return n % d == 0; }
    };

    // any_of 返回bool值。当存在一个为true时，返回true，否则返回false
    if (std::any_of(v.cbegin(), v.cend(), DivisibleBy(7))) {
        auto res = std::find_if(v.cbegin(), v.cend(), DivisibleBy(7));
        std::cout << "At least one number is divisible by 7. last find:" << std::distance(v.cbegin(), res) << std::endl;
    }
}
