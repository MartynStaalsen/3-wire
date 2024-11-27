#ifndef SERICAT_SDO_HPP_
#define SERICAT_SDO_HPP_

#include <cstdint>
#include <stdexcept>
#include <string>
#include <cstring>
#include "sericat_types.hpp"

// define an "sdo" obj
// this will live at an address, but that address is not part of the object
// this will have some data, a description/name tag, and a type
// the type will type will be used to determine how to interpret the data, but this will not be forced:
// data will be stored as a byte array of some type dynamic enough to know its own size

namespace sericat {

// base interface class for sdo objs
class SdoBase {
protected:
  std::string data_ = "";

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
  void deserialize(std::string const& sdo_str){
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
