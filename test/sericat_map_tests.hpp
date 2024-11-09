#ifndef SERICAT_MAP_TESTS_HPP_
#define SERICAT_MAP_TESTS_HPP_

#include "gtest/gtest.h"

#include "sericat_sdo_map.hpp"

namespace sericat
{

TEST(SdoMapTest, SdoPtrMapTest){
  SdoMap sdo_map({
    {"Bob", std::make_shared<Sdo<bool>>("Bob", true).get()},
    {"Irma", std::make_shared<Sdo<std::int32_t>>("Irma", -44).get()},
    {"Frank", std::make_shared<Sdo<float>>("Frank", 3.14).get()},
    {"Sue", std::make_shared<Sdo<std::string>>("Sue", "hi there").get()}
  });

  // serialize
  EXPECT_EQ(sdo_map.serialize_sdo("Bob"), to_byte_string(true));
  EXPECT_EQ(sdo_map.serialize_sdo("Irma"), to_byte_string(-44));
  EXPECT_EQ(sdo_map.serialize_sdo("Frank"), to_byte_string(3.14f));
  EXPECT_EQ(sdo_map.serialize_sdo("Sue"), to_byte_string(std::string("hi there")));  // BUG: funky with cstrs still

  // deserialize
  sdo_map.deserialize_sdo("Bob", to_byte_string(false));
  sdo_map.deserialize_sdo("Irma", to_byte_string(44));
  sdo_map.deserialize_sdo("Frank", to_byte_string(6.28f));
  sdo_map.deserialize_sdo("Sue", to_byte_string(std::string("bye there")));  // BUG: funky with cstrs still

  // check by casting to original type cast
  EXPECT_EQ(static_cast<Sdo<bool>*>(sdo_map.at("Bob"))->get(), false);
  EXPECT_EQ(static_cast<Sdo<std::int32_t>*>(sdo_map.at("Irma"))->get(), 44);
  EXPECT_EQ(static_cast<Sdo<float>*>(sdo_map.at("Frank"))->get(), 6.28f);
  EXPECT_EQ(static_cast<Sdo<std::string>*>(sdo_map.at("Sue"))->get(), "bye there");
}

TEST(SdoMapTest, StaticSdoMapTest){

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
  EXPECT_EQ(sdo_map.serialize_sdo("Bob"), to_byte_string(true));
  EXPECT_EQ(sdo_map.serialize_sdo("Irma"), to_byte_string(123));
  EXPECT_EQ(sdo_map.serialize_sdo("Frank"), to_byte_string(123.456f));
  EXPECT_EQ(sdo_map.serialize_sdo("Sue"), to_byte_string(std::string("Hello, World!")));  // BUG: funky with cstrs still

  // write to original sdos
  some_bool.set(false);
  some_int.set(456);
  some_float.set(456.789f);
  some_string.set("Goodbye, World!");

  // check
  EXPECT_EQ(sdo_map.serialize_sdo("Bob"), to_byte_string(false));
  EXPECT_EQ(sdo_map.serialize_sdo("Irma"), to_byte_string(456));
  EXPECT_EQ(sdo_map.serialize_sdo("Frank"), to_byte_string(456.789f));
  EXPECT_EQ(sdo_map.serialize_sdo("Sue"), to_byte_string(std::string("Goodbye, World!")));  // BUG: funky with cstrs still
}

TEST(SdoMapTest, MapSerDeser){
  SdoMap sdo_map({
    {"Bob", new Sdo<bool>("Bob", true)},
    {"Irma", new Sdo<std::int32_t>("Irma", -44)},
    {"Frank", new Sdo<float>("Frank", 3.14)},
    {"Sue", new Sdo<std::string>("Sue", "hi there")}
  });

  // serialize
  StringMap expected = {
    {"Bob", to_byte_string(true)},
    {"Irma", to_byte_string(-44)},
    {"Frank", to_byte_string(3.14f)},
    {"Sue", to_byte_string(std::string("hi there"))}
  };

  EXPECT_EQ(sdo_map.serialize(), to_byte_string(expected));

  // deserialize
  StringMap new_expected = {
    {"Bob", to_byte_string(false)},
    {"Irma", to_byte_string(44)},
    {"Frank", to_byte_string(6.28f)},
    {"Sue", to_byte_string(std::string("bye there"))}
  };

  sdo_map.deserialize(to_byte_string(new_expected));

  // check by casting to original type cast
  EXPECT_EQ(static_cast<Sdo<bool>*>(sdo_map.at("Bob"))->get(), false);
  EXPECT_EQ(static_cast<Sdo<std::int32_t>*>(sdo_map.at("Irma"))->get(), 44);
  EXPECT_EQ(static_cast<Sdo<float>*>(sdo_map.at("Frank"))->get(), 6.28f);
  EXPECT_EQ(static_cast<Sdo<std::string>*>(sdo_map.at("Sue"))->get(), "bye there");
}

}   // namespace sericat

#endif // SERICAT_MAP_TESTS_HPP_
