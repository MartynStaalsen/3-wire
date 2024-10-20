#include "helper_unit_tests.hpp"
#include "sim_test.hpp"

#include "gtest/gtest.h"

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
