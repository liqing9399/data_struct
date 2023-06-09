/***************************************************
#filename      : test_embedding.cpp
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-06 17:48:30
#last modified : 2023-06-08 10:39:40
#description   : NA
***************************************************/
#include "gtest/gtest.h"
#include "../include/net.h"
#include <vector>

TEST(example, sum_zero)
{
  std::vector<int> integers = {1, -1, 2, -2, 3, -3};
  auto result = std::accumulate(integers.begin(), integers.end(), 0, std::plus<int>());
  ASSERT_EQ(result, 0);
}

TEST(example, sum_five)
{
  auto integers = {1, 2, 3, 4, 5};
  auto result = std::accumulate(integers.begin(), integers.end(), 0, std::plus<int>());
  ASSERT_EQ(result, 15);
}

