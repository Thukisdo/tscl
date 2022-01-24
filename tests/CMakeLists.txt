
find_package(GTest REQUIRED)

add_executable(ThreadPoolTest ThreadPoolTest.cpp)
target_link_libraries(ThreadPoolTest tscl GTest::GTest GTest::gtest_main)

gtest_discover_tests(ThreadPoolTest)
