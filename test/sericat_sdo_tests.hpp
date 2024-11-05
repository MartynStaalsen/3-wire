#ifndef SERICAT_SDO_TESTS_HPP_
#define SERICAT_SDO_TESTS_HPP_

#include "gtest/gtest.h"

#include "sericat_sdo.hpp"
#include "sericat_sdo_map.hpp"

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

  // try geting/writing
  const bool bool_val = true;
  const std::int32_t int_val = 123;
  const float float_val = 123.456f;
  const std::string string_val = "Hello, World!";

  // check description getting 
  EXPECT_EQ(Bob->description_, "Bob");
  EXPECT_EQ(Irma->description_, "Irma");
  EXPECT_EQ(Frank->description_, "Frank");
  EXPECT_EQ(Sue->description_, "Sue");

  //try again but from the vector
  EXPECT_EQ(sdo_vector[0]->description_, "Bob");
  EXPECT_EQ(sdo_vector[1]->description_, "Irma");
  EXPECT_EQ(sdo_vector[2]->description_, "Frank");
  EXPECT_EQ(sdo_vector[3]->description_, "Sue");

  // same for types
  EXPECT_EQ(Bob->type_, SdoDataType::BOOL);
  EXPECT_EQ(Irma->type_, SdoDataType::INT_32);
  EXPECT_EQ(Frank->type_, SdoDataType::FLOAT);
  EXPECT_EQ(Sue->type_, SdoDataType::STRING);

  //try again but from the vector
  EXPECT_EQ(sdo_vector[0]->type_, SdoDataType::BOOL);
  EXPECT_EQ(sdo_vector[1]->type_, SdoDataType::INT_32);
  EXPECT_EQ(sdo_vector[2]->type_, SdoDataType::FLOAT);
  EXPECT_EQ(sdo_vector[3]->type_, SdoDataType::STRING);

  // cast these values to strings 
  // like literally interpret their bytes as chars
  // and store them in the data_ field
  sdo_vector[0]->deserialize(to_byte_string<bool>(bool_val));
  sdo_vector[1]->deserialize(to_byte_string<std::int32_t>(int_val));
  sdo_vector[2]->deserialize(to_byte_string<float>(float_val));
  sdo_vector[3]->deserialize(to_byte_string<std::string>(string_val));

  EXPECT_EQ(Bob->get(), true);
  EXPECT_EQ(Irma->get(), 123);
  EXPECT_EQ(Frank->get(), 123.456f);
  EXPECT_EQ(Sue->get(), "Hello, World!");

  // try decoding data to string
  EXPECT_EQ(sdo_vector[0]->serialize(), to_byte_string(bool_val));
  EXPECT_EQ(sdo_vector[1]->serialize(), to_byte_string(int_val));
  EXPECT_EQ(sdo_vector[2]->serialize(), to_byte_string(float_val));
  EXPECT_EQ(sdo_vector[3]->serialize(), to_byte_string(string_val));
  EXPECT_EQ(sdo_vector[3]->serialize(), string_val);  // should be itself

  // try typed writing (new values)
  Bob->set(false);
  Irma->set(456);
  Frank->set(456.789f);
  Sue->set("Goodbye, World!");
  
  EXPECT_EQ(Bob->get(), false);
  EXPECT_EQ(Irma->get(), 456);
  EXPECT_EQ(Frank->get(), 456.789f);
  EXPECT_EQ(Sue->get(), "Goodbye, World!");

  // try decoding data to string
  EXPECT_EQ(sdo_vector[0]->serialize(), to_byte_string(false));
  EXPECT_EQ(sdo_vector[1]->serialize(), to_byte_string(456));
  EXPECT_EQ(sdo_vector[2]->serialize(), to_byte_string(456.789f));
  EXPECT_EQ(sdo_vector[3]->serialize(), to_byte_string(std::string("Goodbye, World!")));  // BUG: funky with cstrs still
  EXPECT_EQ(sdo_vector[3]->serialize(), "Goodbye, World!");  // should be itself
}

TEST(SdoTest, SdoMapTest){
  SdoMap sdo_map({
    {"Bob", std::make_shared<Sdo<bool>>("Bob", true).get()},
    {"Irma", std::make_shared<Sdo<std::int32_t>>("Irma", -44).get()},
    {"Frank", std::make_shared<Sdo<float>>("Frank", 3.14).get()},
    {"Sue", std::make_shared<Sdo<std::string>>("Sue", "hi there").get()}
  });

  // serialize
  EXPECT_EQ(sdo_map.serializ_sdo("Bob"), to_byte_string(true));
  EXPECT_EQ(sdo_map.serializ_sdo("Irma"), to_byte_string(-44));
  EXPECT_EQ(sdo_map.serializ_sdo("Frank"), to_byte_string(3.14f));
  EXPECT_EQ(sdo_map.serializ_sdo("Sue"), to_byte_string(std::string("hi there")));  // BUG: funky with cstrs still

  // deserialize
  sdo_map.deserialize("Bob", to_byte_string(false));
  sdo_map.deserialize("Irma", to_byte_string(44));
  sdo_map.deserialize("Frank", to_byte_string(6.28f));
  sdo_map.deserialize("Sue", to_byte_string(std::string("bye there")));  // BUG: funky with cstrs still

  // check by casting to original type cast
  EXPECT_EQ(static_cast<Sdo<bool>*>(sdo_map.at("Bob"))->get(), false);
  EXPECT_EQ(static_cast<Sdo<std::int32_t>*>(sdo_map.at("Irma"))->get(), 44);
  EXPECT_EQ(static_cast<Sdo<float>*>(sdo_map.at("Frank"))->get(), 6.28f);
  EXPECT_EQ(static_cast<Sdo<std::string>*>(sdo_map.at("Sue"))->get(), "bye there");
}
TEST(SdoTest, StaticSdoMapTest){

  sericat::Sdo<bool> some_bool("some_bool", true);
  sericat::Sdo<std::int32_t> some_int("some_int", 123);
  sericat::Sdo<float> some_float("some_float", 123.456f);
  sericat::Sdo<std::string> some_string("some_string", "Hello, World!");

  SdoMap sdo_map({
    {"Bob", &some_bool},
    {"Irma", &some_int},
    {"Frank", &some_float},
    {"Sue", &some_string}
  });

  // check
  EXPECT_EQ(sdo_map.serializ_sdo("Bob"), to_byte_string(true));
  EXPECT_EQ(sdo_map.serializ_sdo("Irma"), to_byte_string(123));
  EXPECT_EQ(sdo_map.serializ_sdo("Frank"), to_byte_string(123.456f));
  EXPECT_EQ(sdo_map.serializ_sdo("Sue"), to_byte_string(std::string("Hello, World!")));  // BUG: funky with cstrs still

  // write to original sdos
  some_bool.set(false);
  some_int.set(456);
  some_float.set(456.789f);
  some_string.set("Goodbye, World!");

  // check
  EXPECT_EQ(sdo_map.serializ_sdo("Bob"), to_byte_string(false));
  EXPECT_EQ(sdo_map.serializ_sdo("Irma"), to_byte_string(456));
  EXPECT_EQ(sdo_map.serializ_sdo("Frank"), to_byte_string(456.789f));
  EXPECT_EQ(sdo_map.serializ_sdo("Sue"), to_byte_string(std::string("Goodbye, World!")));  // BUG: funky with cstrs still
}

}  // namespace sericat

#endif  // SERICAT_SDO_TESTS_HPP_