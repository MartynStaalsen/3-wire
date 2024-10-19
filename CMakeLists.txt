cmake_minimum_required(VERSION 3.10)
project(black_box)

set(CMAKE_CXX_STANDARD 14)

add_executable(unit_test_executable test/unit_test_driver.cpp)

include(FetchContent)


include(helper_header.hpp)

fetchcontent_declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
fetchcontent_makeavailable(googletest)

enable_testing()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)

target_link_libraries(unit_test_executable
  GTest::gtest_main
)

target_include_directories(unit_test_executable
  PRIVATE ${CMAKE_SOURCE_DIR}
)

include(GoogleTest)
gtest_discover_tests(unit_test_executable)
