#include <gtest/gtest.h>
#include <ThreadPool.hpp>
#include <chrono>

TEST(ThreadPoolTest, test) {

  tscl::ThreadPool pool(4);
  std::function<int(int)> f = [](int i) {
    return i;
  };
  for (int i = 0; i < 100; i++) {
    pool.enqueue(f, i);
  }
}