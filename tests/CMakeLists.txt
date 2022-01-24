
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(ThreadPoolTest ThreadPoolTest.cpp)
target_link_libraries(ThreadPoolTest tscl gtest_main)

gtest_discover_tests(ThreadPoolTest)
