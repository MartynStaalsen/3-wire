
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
  std::string read(){
    return data_;
  }

  // turn a string into my data_
  // throws if string cannot be decoded
  void write(std::string const sdo_str){
    switch (type_)
    {
      case SdoDataType::BOOL:
        // for bool, expect single char
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
        data_ = sdo_str;
        break;
      case SdoDataType::FLOAT:
        // for float, expect 4 chars
        if (sdo_str.size() != sizeof(float))
        {
          throw std::invalid_argument("Cannot decode float from string of size " + std::to_string(sdo_str.size()));
        }
        break;
      case SdoDataType::STRING:
        data_ = sdo_str;
        break;
    }
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
    return data_[0] != 0;
  }

  void write(bool const& value) {
    data_ = std::string(1, value ? 1 : 0);
  }
};

template <>
class Sdo<std::int32_t> : public SdoBase {
public:
  Sdo(std::string const& name) : SdoBase(name, SdoDataType::INT_32) {}

  std::int32_t read() const {
    std::int32_t value;
    std::memcpy(&value, data_.data(), sizeof(std::int32_t));
    return value;
  }

  void write(std::int32_t const& value) {
    data_.resize(sizeof(std::int32_t));
    std::memcpy(&data_[0], &value, sizeof(std::int32_t));
  }
};

template <>
class Sdo<float> : public SdoBase {
public:
  Sdo(std::string const& name) : SdoBase(name, SdoDataType::FLOAT) {}

  float read() const {
    float value;
    std::memcpy(&value, data_.data(), sizeof(float));
    return value;
  }

  void write(float const& value) {
    data_.resize(sizeof(float));
    std::memcpy(&data_[0], &value, sizeof(float));
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