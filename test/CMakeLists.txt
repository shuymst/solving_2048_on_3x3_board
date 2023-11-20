include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(mini2048_test board_test.cc)
target_link_libraries(mini2048_test PRIVATE mini2048 gtest_main)

include(GoogleTest)
gtest_discover_tests(mini2048_test)