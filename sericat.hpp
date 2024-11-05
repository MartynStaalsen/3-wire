
#ifndef SERICCAT_HPP_
#define SERICCAT_HPP_

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

enum class SdoDataType {
  BOOL,
  INT_32,
  FLOAT,
  STRING
};

// make a base class for the sdo obj, that 
class SdoBase {
protected:
  std::string data_;
  
public:
  const std::string name_;
  const SdoDataType type_;

  virtual ~SdoBase() = default;

  SdoBase(std::string const& name, SdoDataType type)
    : name_(name), type_(type) {}

  std::string get_name() const {
    return name_;
  }

  SdoDataType get_type() const {
    return type_;
  }

  // get data as a string
  std::string get_data_string(){
    return data_;
  }

  // turn put raw string into data_. make sure it's valid for type_ first tho
  // throws invalid_argument if the string is not the right size for the type 
  void set_data_string(std::string const sdo_str){
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
class Sdo;

template <>
class Sdo<bool> : public SdoBase {
public:
  Sdo(std::string const& name) : SdoBase(name, SdoDataType::BOOL) {}

  bool read() const {
    return from_byte_string<bool>(data_);
  }

  void write(bool const& value) {
    data_ = to_byte_string<bool>(value);
  }
};

template <>
class Sdo<std::int32_t> : public SdoBase {
public:
  Sdo(std::string const& name) : SdoBase(name, SdoDataType::INT_32) {}

  std::int32_t read() const {
    return from_byte_string<std::int32_t>(data_);
  }

  void write(std::int32_t const& value) {
    data_ = to_byte_string<std::int32_t>(value);
  }
};

template <>
class Sdo<float> : public SdoBase {
public:
  Sdo(std::string const& name) : SdoBase(name, SdoDataType::FLOAT) {}

  float read() const {
    return from_byte_string<float>(data_);
  }

  void write(float const& value) {
    data_ = to_byte_string<float>(value);
  }
};

template <>
class Sdo<std::string> : public SdoBase {
public:
  Sdo(std::string const& name) : SdoBase(name, SdoDataType::STRING) {}

  std::string read() const {
    return data_;
  }

  void write(std::string const& value) {
    data_ = value;
  }
};
}  // namespace sericat

#endif  // SERICCAT_HPP_