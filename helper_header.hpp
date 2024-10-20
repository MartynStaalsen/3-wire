#ifndef helper_header_hpp
#define helper_header_hpp

namespace bb_helpers
{

const char msg_start_sentinel = '?';
const char msg_end_sentinel = '!';

const char chunk_start_sentinel = '[';
const char chunk_end_sentinel = ']';

const char chunk_internal_delimiter = ':';

const std::string kBoolTypestr = "BOOL";
const std::string kInt32Typestr = "I_32";

class ParsingError : public std::exception {
public:
  ParsingError(const std::string& message = "Parsing error occurred")
    : msg_(message) {}

  virtual const char* what() const noexcept override {
    return msg_.c_str();
  }

private:
  std::string msg_;
};

/*
  message is a char array something like this:

"?[a:b:xxx][a:b:yyy][a:b:zzz]!""

  a chunk is a section like a:b:xxx,
  where a is the chunk id, b is the chunk type, and xxx is the chunk data.

  chunks in messages are wrapped by '[' and ']' characters.
*/
enum class ChunkDataType
{
  UNKNOWN,
  BOOL,
  INT_32  // on esp32, int is 32 bits
};

struct Chunk
{
  uint slave_id;
  std::string key;
  ChunkDataType type = ChunkDataType::UNKNOWN;
  std::string data_str;
  bool valid = false;  // if not all fields are valid, this will be false
};

Chunk make_chunk(
  uint slave_id,
  std::string const& key,
  bool bool_val
){
  Chunk chunk;
  chunk.slave_id = slave_id;
  chunk.key = key;
  chunk.type = ChunkDataType::BOOL;
  chunk.data_str = static_cast<char>(bool_val);
  chunk.valid = true;
  return chunk;
}


Chunk make_chunk(
  uint slave_id,
  std::string const& key,
  int32_t int_val
){
  Chunk chunk;
  chunk.slave_id = slave_id;
  chunk.key = key;
  chunk.type = ChunkDataType::BOOL;
  chunk.data_str = reinterpret_cast<const char*>(&int_val), sizeof(int_val);
  chunk.valid = true;
  return chunk;
}

ChunkDataType decode_chunk_type(std::string const& chunk_type_str) {
  if (chunk_type_str == kBoolTypestr)
  {
    return ChunkDataType::BOOL;
  }
  else if (chunk_type_str == kInt32Typestr)
  {
    return ChunkDataType::INT_32;
  }
  else
  {
    throw ParsingError("Unknown chunk type: " + chunk_type_str);
  }
}

std::string encode_chunk_type(ChunkDataType chunk_type) {
  switch (chunk_type)
  {
    case ChunkDataType::BOOL:
      return kBoolTypestr;
    case ChunkDataType::INT_32:
      return kInt32Typestr;
  }
  throw ParsingError();
}

Chunk decode_chunk(std::string const& chunk_str_og) {
  std::string chunk_str = chunk_str_og;
  Chunk chunk;
  try {
    if (chunk_str[0] != chunk_start_sentinel || chunk_str[chunk_str.size() - 1] != chunk_end_sentinel)
    {
      throw ParsingError("Chunk does not start or end with correct sentinel");
    }
    if (chunk_str.size() < 5)
    {
      throw ParsingError("Chunk is too short");
    }
    chunk_str = chunk_str.substr(1, chunk_str.size() - 2);  // strip []'s
    for (std::string t : std::vector<std::string>({"slave_id", "key", "type"})){
      auto next_delip_pos = chunk_str.find(chunk_internal_delimiter);
      if (next_delip_pos == std::string::npos) {
        throw ParsingError("While parsing " + t + " could not find delimiters in chunk data");
      } else if (next_delip_pos == 0) {
        throw ParsingError("Empty field in chunk data");
      }
      std::string token = chunk_str.substr(0, next_delip_pos);
      if (t == "slave_id"){
        chunk.slave_id = std::stoi(token);
      }
      else if (t == "key"){
        chunk.key = token;
      }
      else if (t == "type"){
        chunk.type = decode_chunk_type(token);
      }
      chunk_str = chunk_str.substr(next_delip_pos + 1);
    }
    chunk.data_str = chunk_str;
  } catch (std::exception const& e) {
    chunk.valid = false;
    throw ParsingError(e.what());  // TODO: are we throwing or returning invalid?
  }
  chunk.valid = true;
  return chunk;
}

std::string encode_chunk(Chunk const& chunk) {
  std::string chunk_str;
  chunk_str +=chunk_start_sentinel;
  chunk_str += std::to_string(chunk.slave_id);
  chunk_str += chunk_internal_delimiter;
  chunk_str += encode_chunk_type(chunk.type);
  chunk_str += chunk_internal_delimiter;
  chunk_str += chunk.data_str;
  chunk_str += chunk_end_sentinel;

  return chunk_str;
}

}   // namespace bb_helpers

#endif  // helper_header_hpp
