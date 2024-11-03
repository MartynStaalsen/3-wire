#include "gtest/gtest.h"

#include "stringycat.hpp"

namespace stringycat
{

TEST(SdoTest, SdoBase)
{
  std::vector<std::shared_ptr<SdoBase>> sdo_vector;
  sdo_vector.push_back(std::make_shared<Sdo<bool>>("Bob"));
  sdo_vector.push_back(std::make_shared<Sdo<std::int32_t>>("Irma"));
  sdo_vector.push_back(std::make_shared<Sdo<float>>("Frank"));
  sdo_vector.push_back(std::make_shared<Sdo<std::string>>("Sue"));}

} // namespace stringycat