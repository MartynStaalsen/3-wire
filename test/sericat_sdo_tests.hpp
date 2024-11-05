#ifndef SERICAT_SDO_TESTS_HPP_
#define SERICAT_SDO_TESTS_HPP_

#include "gtest/gtest.h"

#include "sericat_sdo.hpp"

namespace sericat
{

TEST(SdoTest, SdoBase)
{
  std::vector<std::shared_ptr<SdoBase>> sdo_vector;

  auto Bob = std::make_shared<Sdo<bool>>("Bob");
  auto Irma = std::make_shared<Sdo<std::int32_t>>("Irma");
  auto Frank = std::make_shared<Sdo<float>>("Frank");
  auto Sue = std::make_shared<Sdo<std::string>>("Sue");

  sdo_vector.push_back(Bob);
  sdo_vector.push_back(Irma);
  sdo_vector.push_back(Frank);
  sdo_vector.push_back(Sue);

  // try reading/writing
  const bool bool_val = true;
  const std::int32_t int_val = 123;
  const float float_val = 123.456f;
  const std::string string_val = "Hello, World!";

  // cast these values to strings 
  // like literally interpret their bytes as chars
  // and store them in the data_ field


  sdo_vector[0]->set_data_string(to_byte_string<bool>(bool_val));
  sdo_vector[1]->set_data_string(to_byte_string<std::int32_t>(int_val));
  sdo_vector[2]->set_data_string(to_byte_string<float>(float_val));
  sdo_vector[3]->set_data_string(to_byte_string<std::string>(string_val));


  EXPECT_EQ(Bob->read(), true);
  EXPECT_EQ(Irma->read(), 123);
  EXPECT_EQ(Frank->read(), 123.456f);
  EXPECT_EQ(Sue->read(), "Hello, World!");

  // try decoding data to string
  EXPECT_EQ(sdo_vector[0]->get_data_string(), to_byte_string(bool_val));
  EXPECT_EQ(sdo_vector[1]->get_data_string(), to_byte_string(int_val));
  EXPECT_EQ(sdo_vector[2]->get_data_string(), to_byte_string(float_val));
  EXPECT_EQ(sdo_vector[3]->get_data_string(), to_byte_string(string_val));
  EXPECT_EQ(sdo_vector[3]->get_data_string(), string_val);  // should be itself

  // try typed writing (new values)
  Bob->write(false);
  Irma->write(456);
  Frank->write(456.789f);
  Sue->write("Goodbye, World!");
  
  EXPECT_EQ(Bob->read(), false);
  EXPECT_EQ(Irma->read(), 456);
  EXPECT_EQ(Frank->read(), 456.789f);
  EXPECT_EQ(Sue->read(), "Goodbye, World!");

  // try decoding data to string
  EXPECT_EQ(sdo_vector[0]->get_data_string(), to_byte_string(false));
  EXPECT_EQ(sdo_vector[1]->get_data_string(), to_byte_string(456));
  EXPECT_EQ(sdo_vector[2]->get_data_string(), to_byte_string(456.789f));
  EXPECT_EQ(sdo_vector[3]->get_data_string(), to_byte_string(std::string("Goodbye, World!")));  // BUG: funky with cstrs still
  EXPECT_EQ(sdo_vector[3]->get_data_string(), "Goodbye, World!");  // should be itself
}

}  // namespace sericat

#endif  // SERICAT_SDO_TESTS_HPP_