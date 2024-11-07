#ifndef SERICCAT_SDO_HPP_
#define SERICCAT_SDO_HPP_

#include <cstdint>
#include <stdexcept>
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

enum SdoDataType : char {
  BOOL,
  INT_32,
  FLOAT,
  STRING,
  STRING_LIST
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

const char kDelim = '\0';

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
// TODO: should probably throw if string has the delim char in it...
template <>
std::string to_byte_string<StringList>(StringList const& value){
  std::string result;
  for (std::size_t i = 0; i < value.size(); ++i) {
    result += value[i] + kDelim;
  }
  return result;
}

template <>
StringList from_byte_string<StringList>(std::string const& s){
  StringList result;
  std::string::size_type start = 0;
  std::string::size_type end = std::string::npos;
  while (true) {
    end = s.find(kDelim, start);
    if (end == std::string::npos) {
      break;
    }
    result.push_back(s.substr(start, end - start));
    start = end + 1;
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

  std::string serialize_prototype(){
    return to_byte_string(type_) + to_byte_string(description_);
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
  Sdo(std::string const& description, T const& initial_value) : SdoBase(description, SdoDataType::STRING) {
    set(initial_value);
  }

  T get() const {
    return from_byte_string<T>(data_);
  }

  void set(T const& value) {
    data_ = to_byte_string<T>(value);
  }
};


}  // namespace sericat

#endif  // SERICCAT_SDO_HPP_
