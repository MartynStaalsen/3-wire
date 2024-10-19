#ifndef helper_header_hpp
#define helper_header_hpp

namespace bb_helpers
{

const char msg_start_sentinel = '?';
const char msg_end_sentinel = '!';

const char chunk_start_sentinel = '[';
const char chunk_end_sentinel = ']';

const char chunk_internal_delimiter = ':';

const static inline std::string bool_typestr = "BOOL";
const static inline std::string int_32_typestr = "I_32";
const static inline std::string unknown_typestr = "UNKN";

/*
  message is a char array something like this:

"?[a:b:xxx][a:b:yyy][a:b:zzz]!""

  a chunk is a section like a:b:xxx,
  where a is the chunk id, b is the chunk type, and xxx is the chunk data.

  chunks in messages are wrapped by '[' and ']' characters.
*/
enum class ChunkDataType
{
  BOOL,
  INT_32,  // on esp32, int is 32 bits
  UNKNOWN  // use this type if parsing fails, i guess
};

struct Chunk
{
  uint id;
  std::string type;
  std::string data;
};

// chunk id string can be multple characters. parse it to a uint
uint decode_chunk_id(std::string const& chunk_id_str)
{
  return std::stoul(chunk_id_str);
}

ChunkDataType decode_chunk_type(std::string const& chunk_type_str)
{
  if (chunk_type_str == bool_typestr)
  {
    return ChunkDataType::BOOL;
  }
  else if (chunk_type_str == int_32_typestr)
  {
    return ChunkDataType::INT_32;
  }
  else
  {
    return ChunkDataType::UNKNOWN;
  }
}

std::string encode_chunk_type(ChunkDataType chunk_type)
{
  switch (chunk_type)
  {
    case ChunkDataType::BOOL:
      return bool_typestr;
    case ChunkDataType::INT_32:
      return int_32_typestr;
    default:
      return unknown_typestr;
  }
}

// helper to turn a message into a vector of chunks
std::vector<std::string> make_chunks(std::string const& message)
{
  std::vector<std::string> chunks;
  std::string chunk;
  bool in_chunk = false;
  for (auto const& c : message)
  {
    if (c == chunk_start_sentinel)
    {
      in_chunk = true;
      chunk.clear();
    }
    else if (c == chunk_end_sentinel)
    {
      in_chunk = false;
      chunks.push_back(chunk);
    }
    else if (in_chunk)
    {
      chunk.push_back(c);
    }
  }
  return chunks;
}

// helper to turn a chunk into a vector of data
std::vector<std::string> make_data(std::string const& chunk)
{
  std::vector<std::string> data;
  std::string datum;
  bool in_datum = false;
  for (auto const& c : chunk)
  {
    if (c == chunk_internal_delimiter)
    {
      in_datum = true;
    }
    else if (c == chunk_end_sentinel)
    {
      in_datum = false;
      data.push_back(datum);
    }
    else if (in_datum)
    {
      datum.push_back(c);
    }
  }
  return data;
}


}   // namespace bb_helpers

#endif  // helper_header_hpp
