#include "simulator/sim_test.hpp"
#include "sericat_sdo_tests.hpp"
#include "sericat_map_tests.hpp"
#include "sericat_slave_tests.hpp"

#include "gtest/gtest.h"

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
