#ifndef SERICAT_SDO_HPP_
#define SERICAT_SDO_HPP_

#include <cstdint>
#include <stdexcept>
#include <map>
#include <string>
#include <cstring>
#include <vector>

// define an "sdo" obj
// this will live at an address, but that address is not part of the object
// this will have some data, a description/name tag, and a type
// the type will type will be used to determine how to interpret the data, but this will not be forced:
// data will be stored as a byte array of some type dynamic enough to know its own size

namespace sericat {


using StringList = std::vector<std::string>;
using StringMap = std::map<std::string, std::string>;

enum SdoDataType : char {
  BOOL = 'B',
  INT_32 = 'I',
  FLOAT = 'F',
  STRING = 'S',
  STRING_LIST = 's'
};

template <typename T>
SdoDataType t_to_type();

template <>
SdoDataType t_to_type<bool>(){
  return SdoDataType::BOOL;
}

template <>
SdoDataType t_to_type<std::int32_t>(){
  return SdoDataType::INT_32;
}

template <>
SdoDataType t_to_type<float>(){
  return SdoDataType::FLOAT;
}

template <>
SdoDataType t_to_type<std::string>(){
  return SdoDataType::STRING;
}

template <>
SdoDataType t_to_type<StringList>(){
  return SdoDataType::STRING_LIST;
}

template <typename T>
std::string to_byte_string(T const& value){
  return std::string(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename T>
T from_byte_string(std::string const& s){
  T value;
  std::memcpy(&value, s.data(), sizeof(T));
  return value;
}

// Specialization for std::string
template <>
std::string to_byte_string<std::string>(std::string const& value){
  return value;
}

template <>
std::string from_byte_string<std::string>(std::string const& s){
  return s;
}

// specialize for string list
template <>
std::string to_byte_string<StringList>(StringList const& value){
  if (value.size() > 0xff) {
    throw std::invalid_argument("StringList too long to serialize");
  }
  char size = static_cast<char>(value.size());
  std::string result(1, size);
  for (std::size_t i = 0; i < value.size(); ++i) {
    // put a char for the size of the string, then the string
    result.push_back(static_cast<char>(value[i].size()));
    result += value[i];
  }
  return result;
}

template <>
StringList from_byte_string<StringList>(std::string const& s){
  StringList result;
  // use first char of s to determine length of the list
  char size = static_cast<char>(s[0]);
  std::size_t i = 1;
  while (i < s.size()) {
    // get the size of the string
    char str_size = s[i];
    // get the string
    result.push_back(s.substr(i + 1, str_size));
    // move to the next string
    i += str_size + 1;
  }
  return result;
}

// specialize for string map
template <>
std::string to_byte_string<StringMap>(StringMap const& value){
  if (value.size() > 0xff) {
    throw std::invalid_argument("StringMap too long to serialize");
  }
  char size = static_cast<char>(value.size());
  std::string result(1, size);

  // store each key-value pair as a StringList of length 2
  for (auto const& [key, val] : value) {
    std::string substring = to_byte_string(StringList({key, val}));
    result.append(substring);
  }
  return result;
}

template <>
StringMap from_byte_string<StringMap>(std::string const& s){
  StringMap result;
  // use first char of s to determine length of the map
  char size = s[0];
  std::size_t ndx = 1;
  for (std::size_t i = 0; i < size; ++i) {
    // get the key-value pair as a StringList
    std::string substr = s.substr(ndx);
    StringList key_val = from_byte_string<StringList>(substr);
    result[key_val[0]] = key_val[1];
    // move to the next key-value pair
    ndx += key_val[0].size() + key_val[1].size() + 2 + 1;
  }
  return result;
}

// make a base class for the sdo obj, that
class SdoBase {
protected:
  std::string data_;

public:
  const std::string description_;
  const SdoDataType type_;

  virtual ~SdoBase() = default;

  SdoBase(std::string const& description, SdoDataType type)
    : description_(description), type_(type) {}

  // get data as a string
  std::string serialize(){
    return data_;
  }

  // turn put raw string into data_. make sure it's valid for type_ first tho
  // throws invalid_argument if the string is not the right size for the type
  void deserialize(std::string const sdo_str){
    switch (type_)
    {
      case SdoDataType::BOOL:
        // for bool, expect single char: ("0", or "1")
        if (sdo_str.size() != 1)
        {
          throw std::invalid_argument("Cannot decode bool from string of size " + std::to_string(sdo_str.size()));
        }
        break;
      case SdoDataType::INT_32:
        // for int, expect 4 chars
        if (sdo_str.size() != sizeof(std::int32_t))
        {
          throw std::invalid_argument("Cannot decode int_32 from string of size " + std::to_string(sdo_str.size()));
        }
        break;
      case SdoDataType::FLOAT:
        // for float, expect 4 chars
        if (sdo_str.size() != sizeof(float))
        {
          throw std::invalid_argument("Cannot decode float from string of size " + std::to_string(sdo_str.size()));
        }
        break;
      case SdoDataType::STRING:
        // don't care about size
        break;
    }
    data_ = sdo_str;
  }

};

// specialize template types for each datatype
template <typename T>
class Sdo : public SdoBase {
public:
  Sdo(std::string const& description) : SdoBase(description, t_to_type<T>()) {}
  Sdo(std::string const& description, T const& initial_value) : SdoBase(description, t_to_type<T>()) {
    set(initial_value);
  }

  T get() const {
    return from_byte_string<T>(data_);
  }

  void set(T const& value) {
    data_ = to_byte_string<T>(value);
  }
};

std::string serialize_prototype(SdoBase const& sdo){
  return to_byte_string(sdo.type_) + to_byte_string(sdo.description_);
}

SdoBase deserialize_prototype(std::string const& sdo_str){
  SdoDataType type = from_byte_string<SdoDataType>(sdo_str.substr(0, sizeof(SdoDataType)));  // SdoDataType is a char
  std::string description = sdo_str.substr(sizeof(SdoDataType));
  return SdoBase(description, type);
}

template<typename T>
T get(SdoBase const& sdo){
  if (sdo.type_ != t_to_type<T>()) {
    throw std::invalid_argument("Cannot get " + sdo.description_ + " as type " + typeid(T).name() + "(" + std::to_string(t_to_type<T>()) + " != " + std::to_string(sdo.type_) + ")");
  }
  return static_cast<Sdo<T> const&>(sdo).get();
}

template<typename T>
void set(SdoBase& sdo, T const& value){
  if (sdo.type_ != t_to_type<T>()) {
    throw std::invalid_argument("Cannot set " + sdo.description_ + " as type " + typeid(T).name() + "(" + std::to_string(t_to_type<T>()) + " != " + std::to_string(sdo.type_) + ")");
  }
  static_cast<Sdo<T>&>(sdo).set(value);
}


}  // namespace sericat

#endif  // SERICAT_SDO_HPP_
