#ifndef SERICAT_SDO_TESTS_HPP_
#define SERICAT_SDO_TESTS_HPP_

#include "gtest/gtest.h"

#include "sericat_sdo.hpp"
#include "sericat_sdo_map.hpp"

namespace sericat
{

TEST(SdoTest, T2Type){
  EXPECT_EQ(t_to_type<bool>(), SdoDataType::BOOL);
  EXPECT_EQ(t_to_type<std::int32_t>(), SdoDataType::INT_32);
  EXPECT_EQ(t_to_type<float>(), SdoDataType::FLOAT);
  EXPECT_EQ(t_to_type<std::string>(), SdoDataType::STRING);
  EXPECT_EQ(t_to_type<StringList>(), SdoDataType::STRING_LIST);
}

TEST(SerDeserTest, SerializeDeserialize){
  // test byte string conversion and back for each type
  bool val = true;
  ASSERT_EQ(
    val,
    from_byte_string<bool>(to_byte_string(val))
  );

  std::int32_t int_val = 123;
  ASSERT_EQ(
    int_val,
    from_byte_string<std::int32_t>(to_byte_string(int_val))
  );

  float float_val = 123.456f;
  ASSERT_EQ(
    float_val,
    from_byte_string<float>(to_byte_string(float_val))
  );

  std::string string_val = "Hello, World!";
  ASSERT_EQ(
    string_val,
    from_byte_string<std::string>(to_byte_string(string_val))
  );
}


TEST(SerDeserTest, SerDeserStringMap)
{
  StringMap string_map_val = {
    {"a", "b"},
    {"c", "d"}
  };

  StringMap deser_stringmap = from_byte_string<StringMap>(to_byte_string(string_map_val));
  ASSERT_EQ(
    deser_stringmap["a"],
    string_map_val["a"]
  );
  ASSERT_EQ(
    deser_stringmap["c"],
    string_map_val["c"]
  );

  ASSERT_EQ(
    string_map_val,
    from_byte_string<StringMap>(to_byte_string(string_map_val))
  );

  StringMap empty_string_map_val = {};
  ASSERT_EQ(
    empty_string_map_val,
    from_byte_string<StringMap>(to_byte_string(empty_string_map_val))
  );
}

TEST(SerDeserTest, SerDeserStringList)
{
  StringList string_list_val = {"Hello", "World", "!"};
  ASSERT_EQ(
    string_list_val,
    from_byte_string<StringList>(to_byte_string(string_list_val))
  );
  ASSERT_EQ(
    to_byte_string(string_list_val),
    "\x03\x05Hello\x05World\x01!"
  );

  // test that it throws if the string is too short
  EXPECT_THROW(
    from_byte_string<StringList>("\x03\x05Hello\x05World\x01"),
    std::invalid_argument
  );

  // throw with too few chars in an elem
  EXPECT_THROW(
    from_byte_string<StringList>("\x03\x05Hellodd\x05World\x01"),
    std::invalid_argument
  );

  // test that it does not throw if the string is too long
  EXPECT_NO_THROW(
    from_byte_string<StringList>("\x03\x05Hello\x05World\x01 blablabla")
  );

  StringList empty_string_list_val = {};
  ASSERT_EQ(
    empty_string_list_val,
    from_byte_string<StringList>(to_byte_string(empty_string_list_val))
  );

  std::string zero_string = std::string(1, static_cast<char>(0));
  ASSERT_EQ(
    to_byte_string(empty_string_list_val),
    zero_string
  );
}

TEST(SerDeserTest, SerDeserSdoDataType){
  SdoDataType bool_type = SdoDataType::BOOL;
  SdoDataType int_type = SdoDataType::INT_32;
  SdoDataType float_type = SdoDataType::FLOAT;
  SdoDataType string_type = SdoDataType::STRING;
  SdoDataType string_list_type = SdoDataType::STRING_LIST;

  ASSERT_EQ(
    bool_type,
    from_byte_string<SdoDataType>(to_byte_string(bool_type))
  );

  ASSERT_EQ(
    int_type,
    from_byte_string<SdoDataType>(to_byte_string(int_type))
  );

  ASSERT_EQ(
    float_type,
    from_byte_string<SdoDataType>(to_byte_string(float_type))
  );

  ASSERT_EQ(
    string_type,
    from_byte_string<SdoDataType>(to_byte_string(string_type))
  );

  ASSERT_EQ(
    string_list_type,
    from_byte_string<SdoDataType>(to_byte_string(string_list_type))
  );
}

TEST(SdoTest, SdoBool){
  Sdo<bool> Bob("Bob", true);
  Sdo<bool> Brian("Brian", false);

  EXPECT_EQ(Bob.get(), true);
  EXPECT_EQ(Brian.get(), false);

  Bob.set(false);
  Brian.set(true);

  EXPECT_EQ(Bob.get(), false);
  EXPECT_EQ(Brian.get(), true);

  Brian.deserialize(Bob.serialize());
  EXPECT_EQ(Brian.get(), Bob.get());
}

TEST(SdoTest, SdoInt){
  Sdo<std::int32_t> Ian("Ian", 123);
  Sdo<std::int32_t> Irma("Irma", -123);

  EXPECT_EQ(Ian.get(), 123);
  EXPECT_EQ(Irma.get(), -123);

  Ian.set(-123);
  Irma.set(123);

  EXPECT_EQ(Ian.get(), -123);
  EXPECT_EQ(Irma.get(), 123);

  Irma.deserialize(Ian.serialize());
  EXPECT_EQ(Irma.get(), Ian.get());
}

TEST(SdoTest, SdoFloat){
  Sdo<float> Fred("Fred", 123.456f);
  Sdo<float> Frankie("Frankie", -123.456f);

  EXPECT_EQ(Fred.get(), 123.456f);
  EXPECT_EQ(Frankie.get(), -123.456f);

  Fred.set(-123.456f);
  Frankie.set(123.456f);

  EXPECT_EQ(Fred.get(), -123.456f);
  EXPECT_EQ(Frankie.get(), 123.456f);

  Frankie.deserialize(Fred.serialize());
  EXPECT_EQ(Frankie.get(), Fred.get());
}

TEST(SdoTest, SdoString){
  Sdo<std::string> Steve("Steve", "Hello, World!");
  Sdo<std::string> Sue("Irma", "Goodbye, World!");

  EXPECT_EQ(Steve.get(), "Hello, World!");
  EXPECT_EQ(Sue.get(), "Goodbye, World!");

  Steve.set("Goodbye, World!");
  Sue.set("Hello, World!");

  EXPECT_EQ(Steve.get(), "Goodbye, World!");
  EXPECT_EQ(Sue.get(), "Hello, World!");

  Sue.deserialize(Steve.serialize());
  EXPECT_EQ(Sue.get(), Steve.get());
}

TEST(SdoTest, SdoStringList){
  Sdo<std::vector<std::string>> Sylvester("Sylvester", {"Hello", "World", "!"});
  Sdo<std::vector<std::string>> Sylvie("Sylvie", {"Goodbye"});
  Sdo<StringList> Syl("Syl", {});

  EXPECT_EQ(Sylvester.get(), std::vector<std::string>({"Hello", "World", "!"}));
  EXPECT_EQ(Sylvie.get(), std::vector<std::string>({"Goodbye"}));
  EXPECT_EQ(Syl.get(), StringList({}));

  // empty one should make an empty list
  Sdo<std::vector<std::string>> unset("empty");
  EXPECT_EQ(unset.get(), std::vector<std::string>());
}

TEST(SdoTest, SdoBaseVector)
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

TEST(SdoTest, PrototypeSerDeser){
  Sdo<bool> Bob("Bob", true);
  Sdo<std::int32_t> Irma("Irma", 123);
  Sdo<float> Frank("Frank", 123.456f);
  Sdo<std::string> Sue("Sue", "Hello, World!");
  Sdo<StringList> Sylvester("Sylvester", {"Hello", "World", "!"});

  std::vector<std::shared_ptr<SdoBase>> sdo_vector;
  // SdoBase newBob = deserialize_prototype(serialize_prototype(Bob));
  // push back from prototype

  sdo_vector.push_back(std::make_shared<SdoBase>(deserialize_prototype(serialize_prototype(Bob))));
  sdo_vector.push_back(std::make_shared<SdoBase>(deserialize_prototype(serialize_prototype(Irma))));
  sdo_vector.push_back(std::make_shared<SdoBase>(deserialize_prototype(serialize_prototype(Frank))));
  sdo_vector.push_back(std::make_shared<SdoBase>(deserialize_prototype(serialize_prototype(Sue))));
  sdo_vector.push_back(std::make_shared<SdoBase>(deserialize_prototype(serialize_prototype(Sylvester))));

  // import data
  sdo_vector[0]->deserialize(Bob.serialize());  // don't need to know type explicitly to do this
  sdo_vector[1]->deserialize(Irma.serialize());
  sdo_vector[2]->deserialize(Frank.serialize());
  sdo_vector[3]->deserialize(Sue.serialize());
  sdo_vector[4]->deserialize(Sylvester.serialize());

  // check by casting to appropriate type
  EXPECT_EQ(static_cast<Sdo<bool>*>(sdo_vector[0].get())->get(), Bob.get());
  EXPECT_EQ(static_cast<Sdo<std::int32_t>*>(sdo_vector[1].get())->get(), Irma.get());
  EXPECT_EQ(static_cast<Sdo<float>*>(sdo_vector[2].get())->get(), Frank.get());
  EXPECT_EQ(static_cast<Sdo<std::string>*>(sdo_vector[3].get())->get(), Sue.get());
  EXPECT_EQ(static_cast<Sdo<StringList>*>(sdo_vector[4].get())->get(), Sylvester.get());
}

TEST(SdoTest, SdoBaseStringList){
  auto Sylvester = std::make_shared<Sdo<std::vector<std::string>>>("Sylvester");

  std::vector<std::string> string_list_val = {"Hello", "World", "!"};
  Sylvester->set(string_list_val);

  EXPECT_EQ(Sylvester->get(), string_list_val);
  std::string serialized = Sylvester->serialize();

  auto Sylvie = std::make_shared<Sdo<std::vector<std::string>>>("Sylvie");
  Sylvie->deserialize(serialized);
  EXPECT_EQ(Sylvie->get(), Sylvester->get());
}

TEST(SdoTest, GenericGetSet){
  Sdo<bool> Bob("Bob", true);
  Sdo<std::int32_t> Irma("Irma", 123);
  Sdo<float> Frank("Frank", 123.456f);
  Sdo<std::string> Sue("Sue", "Hello, World!");

  EXPECT_EQ(get<bool>(Bob), true);
  EXPECT_EQ(get<std::int32_t>(Irma), 123);
  EXPECT_EQ(get<float>(Frank), 123.456f);
  EXPECT_EQ(get<std::string>(Sue), "Hello, World!");

  set<bool>(Bob, false);
  set(Irma, 456);
  set(Frank, 456.789f);
  set<std::string>(Sue, "Goodbye, World!");

  EXPECT_EQ(get<bool>(Bob), false);
  EXPECT_EQ(get<std::int32_t>(Irma), 456);
  EXPECT_EQ(get<float>(Frank), 456.789f);
  EXPECT_EQ(get<std::string>(Sue), "Goodbye, World!");

  EXPECT_THROW(get<std::int32_t>(Bob), std::invalid_argument);
  EXPECT_THROW(set(Bob, 123), std::invalid_argument);

  EXPECT_THROW(get<bool>(Irma), std::invalid_argument);
  EXPECT_THROW(set(Irma, true), std::invalid_argument);

  EXPECT_THROW(get<float>(Sue), std::invalid_argument);
  EXPECT_THROW(set(Sue, 123.456f), std::invalid_argument);

  EXPECT_THROW(get<std::string>(Frank), std::invalid_argument);
  EXPECT_THROW(set<std::string>(Frank, "Hello, World!"), std::invalid_argument);
}

}  // namespace sericat

#endif  // SERICAT_SDO_TESTS_HPP_
