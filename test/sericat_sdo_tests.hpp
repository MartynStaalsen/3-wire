#ifndef SERICAT_SDO_TESTS_HPP_
#define SERICAT_SDO_TESTS_HPP_

#include "gtest/gtest.h"

#include "sericat.hpp"

namespace sericat
{

TEST(SdoTest, SdoBase)
{
  std::vector<std::shared_ptr<SdoBase>> sdo_vector;
  sdo_vector.push_back(std::make_shared<Sdo<bool>>("Bob"));
  sdo_vector.push_back(std::make_shared<Sdo<std::int32_t>>("Irma"));
  sdo_vector.push_back(std::make_shared<Sdo<float>>("Frank"));
  sdo_vector.push_back(std::make_shared<Sdo<std::string>>("Sue"));

  // try reading/writing
  sdo_vector[0]->write("1");
  sdo_vector[1]->write("123");
  sdo_vector[2]->write("123.456");
  sdo_vector[3]->write("Hello, World!");

  EXPECT_EQ(sdo_vector[0]->read<bool>(), true);
  EXPECT_EQ(sdo_vector[1]->read<std::int32_t>(), 123);
  EXPECT_EQ(sdo_vector[2]->read<float>(), 123.456f);
  EXPECT_EQ(sdo_vector[3]->read<std::string>(), "Hello, World!");

  // try decoding data to string
  EXPECT_EQ(sdo_vector[0]->data_to_string(), "1");
  EXPECT_EQ(sdo_vector[1]->data_to_string(), "123");
  EXPECT_EQ(sdo_vector[2]->data_to_string(), "123.456");
  EXPECT_EQ(sdo_vector[3]->data_to_string(), "Hello, World!");

  // try typed writing (new values)
  sdo_vector[0]->write(false);
  sdo_vector[1]->write(456);
  sdo_vector[2]->write(456.789f);
  sdo_vector[3]->write("Goodbye, World!");
  
}

}  // namespace sericat

#endif  // SERICAT_SDO_TESTS_HPP_