#ifndef SERICAT_SDO_MAP_HPP_
#define SERICAT_SDO_MAP_HPP_

#include <sericat_sdo.hpp>

namespace sericat
{
/*
  Define an object that holds sdos. this will be a string map so objects can be accessed by an "index" string
  could just do a list and access by numeric index, but since addresses will be passed as strings over the
  serial line, it makes sense to use strings as the index

  The contents of the map should be statically constructed, though the objects inside can have their data changed ofc

  The SdoMap needs accessors and readers for each sdo type, though they'll be stored as pointers to SdoBase objects
*/

class SdoMap : public std::map<std::string, SdoBase * >
{
public:
  SdoMap() : std::map<std::string, SdoBase * >({}) {}
  SdoMap(std::map<std::string, SdoBase *> sdo_map) : std::map<std::string, SdoBase *>(sdo_map) {}

  // methods to get and set from raw string data
  // these will be used to read and write to the serial line
  void deserialize_sdo(std::string const& key, std::string const& raw_data)
  {
    if (this->find(key) == this->end())
    {
      throw std::runtime_error("Key " + key + " not found in SdoMap");
    }
    this->at(key)->deserialize(raw_data);
  }

  std::string serialize_sdo(std::string const& key)
  {
    return this->at(key)->serialize();
  }

  std::string serialize(){
    // serialize the whole map.
    // do this by converting to a string map, where keys are sdo keys and the values are the serialized sdo
    StringMap result;
    for (auto const& [key, sdo] : *this) {
      result.emplace(key, sdo->serialize());
    }

    return to_byte_string(result);
  }

  void deserialize(std::string const& sdo_map_str){
    // deserialize the whole map
    StringMap string_map = from_byte_string<StringMap>(sdo_map_str);
    for (auto const& [key, raw_data] : string_map) {
      deserialize_sdo(key, raw_data);
    }
  }
};
}  // namespace sericat
#endif  // SERICAT_SDO_MAP_HPP
