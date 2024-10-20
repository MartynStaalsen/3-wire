#include "gtest/gtest.h"

#include "helper_header.hpp"

namespace bb_helpers
{

TEST(BBPRotocolTest, ChunkTypeDecode){
  EXPECT_EQ(decode_chunk_type("BOOL"), ChunkDataType::BOOL);
  EXPECT_EQ(decode_chunk_type("I_32"), ChunkDataType::INT_32);
  EXPECT_THROW(decode_chunk_type("UNKNOWN"), ParsingError);
  EXPECT_EQ(decode_chunk_type(kBoolTypestr), ChunkDataType::BOOL);
  EXPECT_EQ(decode_chunk_type(kInt32Typestr), ChunkDataType::INT_32);

}

TEST(BBProtocolTest, ChunkEncodeDecode)
{
  bb_helpers::Chunk decoded_chunk;
  std::string chunkstr;
  std::vector<std::string> bad_chunks = {
    "",
    ":::",
    "helloworld",
    "1:some_key:BOOL:tru",
    "1:some_key:BOOL:trues",
    "1:some_key:BOOL:123",
    "-1:some_key:BOOL:true",
    "1:some_key:INT_32:true:",
    "1:INT_32:123:",
    "1:some_key:INT_32:123:123",
    "1:some_key:INT_32:123:123:123",
    "[1:2:BOOL:1",
    "1:2:BOOL:1]",
    "[]"
  };

  chunkstr = "[1:2:BOOL:1]";
  decoded_chunk = decode_chunk(chunkstr);
  EXPECT_EQ(decoded_chunk.slave_id, 1);
  EXPECT_EQ(decoded_chunk.key, "2");
  EXPECT_EQ(decoded_chunk.type, ChunkDataType::BOOL);
  EXPECT_TRUE(decoded_chunk.valid);

  chunkstr = "[1:some_key:BOOL:0]";
  decoded_chunk = decode_chunk(chunkstr);
  EXPECT_EQ(decoded_chunk.slave_id, 1);
  EXPECT_EQ(decoded_chunk.key, "some_key");
  EXPECT_EQ(decoded_chunk.type, ChunkDataType::BOOL);
  EXPECT_TRUE(decoded_chunk.valid);


  chunkstr = "[2:i_1:" + kInt32Typestr + ":123]";
  decoded_chunk = decode_chunk(chunkstr);
  EXPECT_EQ(decoded_chunk.slave_id, 2);
  EXPECT_EQ(decoded_chunk.key, "i_1");
  EXPECT_EQ(decoded_chunk.type, ChunkDataType::INT_32);
  EXPECT_TRUE(decoded_chunk.valid);

  for (auto const& bad_chunk : bad_chunks)
  {
    EXPECT_THROW(bb_helpers::decode_chunk(bad_chunk), ParsingError);
  }
}

}   // namespace bb_helpers
