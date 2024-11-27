#ifndef SERICAT_TOKEN_PARSER_HPP
#define SERICAT_TOKEN_PARSER_HPP

#include <functional>
#include <string>
#include <cstdint>

#include "sericat_types.hpp"

namespace sericat
{
/*
  This object breaks incoming data into tokens based on a length byte at the beginning of each token.
  It calls the token_callback_ whenever it has a complete token to pass along.
*/

class TokenParser
{
private:
  std::string accumulator_ = "";  // hold data here between updates
  std::function<void(std::string)> token_callback_;

public:
  explicit TokenParser(
    std::function<void(std::string)> token_callback
  ) : token_callback_(token_callback){}

  ~TokenParser() = default;

  // it is this function's job to collect incoming data from the serial line
  // and parse it into key/value elements
  void update(std::string input){
    accumulator_ += input;

    if (accumulator.size() == 0){
      return
    }
    size_t token_size = acumulator[0];

    if (accumulator_.size() < token_size + 1) {
      return;  // not enough data to fill the token
    }

    std::string token = accumulator_.substr(1, token_size);

    // remove the token from the accumulator
    accumulator_ = accumulator_.substr(token_size + 1);

    token_callback_(token);
  }
};

}  // namespace sericat

#endif  // SERICAT_TOKEN_PARSER_HPP
