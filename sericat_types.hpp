
#ifndef SERICAT_TYPES_HPP_
#define SERICAT_TYPES_HPP_

#include <cstdint>
#include <stdexcept>
#include <map>
#include <string>
#include <cstring>
#include <vector>

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
  char list_len = static_cast<char>(s[0]);
  std::size_t idx = 1;
  while (result.size() < list_len) {
    // get the size of the string
    char str_size = s[idx];
    // get the string
    if (idx + 1 + str_size > s.size()) {
      throw std::invalid_argument("StringList too short to deserialize");
    }
    result.push_back(s.substr(idx + 1, str_size));

    // move to the next string
    idx += str_size + 1;

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

}  // namespace sericat

#endif  // SERICAT_TYPES_HPP_
