#ifndef SERICAT_SLAVE_HPP
#define SERICAT_SLAVE_HPP

#include "sericat_sdo_map.hpp"
#include "sericat_sdo.hpp"
#include "sericat_types.hpp"
#include "sericat_frame_buffer.hpp"

#include <memory>
#include <cstdint>


namespace sericat
{

class SericatSlave {
public:
  // state enum:
  // INIT: initial state, no process or mailbox happen here.
  //  -- transition req: need to know what index i am for mailbox purposes
  // PRE-OP: mailbox can happen, but no process.
  //  -- transition req: need valid pdo assignment
  // OP: mailbox and process can happen
  enum State : std::int32_t {
    INIT = 0,
    PREOP,
    OP,
    ERROR
  };

private:
  Sdo<std::int32_t> index_{"slave_index", 0};  // zero is invalid, this will get set during INIT->PREOP transition
  Sdo<std::int32_t> wkc_{"work_counter", 0};  // working counter, incremented by each device as it's encountered
  Sdo<std::int32_t> state_{"slave_state", INIT};  // current state of the device
  Sdo<std::string> err_msg_{"error_msg", ""};  // error message, if any. Only meaningful in ERROR state

  SdoMap working_map_{{
    {"WKC", &index_},
    {"index", &wkc_},
    {"state", &state_},
    {"error", &err_msg_}
  }}; // holds things like WKC, index, etc

  SdoMap config_;
  SdoMap rxpdo_;
  SdoMap txpdo_;

  std::shared_ptr<Sdo<StringList>> rxpdo_assignment_;  // for reporting purposes
  std::shared_ptr<Sdo<StringList>> txpdo_assignment_;

  SdoMap active_rxpdos_;
  SdoMap active_txpdos_;


  std::string txpdo_callback(std::string key){
    // this should only happen in the OP state
    if (state_.get() != SericatSlave::OP) {
      state_.set(SericatSlave::ERROR);
      err_msg_.set("Cannot write to txpdo while in state " + std::to_string(state_.get()));
    }
    // read the value from the txpdo map
    try{
      return active_txpdos_.serialize_sdo(key);
    } catch (SdoMap::SdoNotFoundException & e) {
      // assuming this is thrown by deserialize_sdo not seeing the key it needs
      state_.set(SericatSlave::ERROR);
      err_msg_.set(e.what());
    }
  }

  void rxpdo_callback(std::string key, std::string data){
    // this should only happen in the OP state
    if (state_.get() != SericatSlave::OP) {
      state_.set(SericatSlave::ERROR);
      err_msg_.set("Cannot read from rxpdo while in state " + std::to_string(state_.get()));
    }
    // write the value to the rxpdo map
    try{
      active_rxpdos_.deserialize_sdo(key, data);
    } catch (SdoMap::SdoNotFoundException & e) {
      // assuming this is thrown by serialize_sdo not seeing the key it needs
      state_.set(SericatSlave::ERROR);
      err_msg_.set(e.what());
    }
  }

  std::deque<std::string> token_buffer_; // holds tokens until they can be processed

  void token_callback(std::string token){
    token_buffer_.push_back(token);

    //
  }

  // std::string element_callback(std::string key, std::string data){
  //   throw std::runtime_error("Not implemented");

  //   // txpdo case
  //   rxpdo_callback(key, data);
  //   return data;

  //   // rxpdo case
  //   return txpdo_callback(key);
  // }

  TokenParser token_parser_ = TokenParser(
    std::bind(&SericatSlave::token_callback, this, std::placeholders::_1));

public:


  // constructor
  // at construct time, contents (but not data) of
  // config, rxpdo, and txpdo maps must be set

  static const inline std::string kRxPDOAssignmentKey = "RxPDO_ASSIGN";
  static const inline std::string kTxPDOAssignmentKey = "TxPDO_ASSIGN";

  std::deque<std::pair<std::string, std::string>> data_queue

  SericatSlave(
    SdoMap const& config,
    SdoMap const& rxpdo,
    SdoMap const& txpdo
  ) : config_(config), rxpdo_(rxpdo), txpdo_(txpdo), active_rxpdos_(), active_txpdos_() {
    // add empty string_list to config for rx and tx pdo assignments
    rxpdo_assignment_ = std::make_shared<Sdo<StringList>>(kRxPDOAssignmentKey);
    txpdo_assignment_ = std::make_shared<Sdo<StringList>>(kTxPDOAssignmentKey);
    // throw if
    if (config_.find(kRxPDOAssignmentKey) != config_.end()) {
      throw std::runtime_error(kRxPDOAssignmentKey + "found prematurely in config");
    }
    if (config_.find(kTxPDOAssignmentKey) != config_.end()) {
      throw std::runtime_error(kTxPDOAssignmentKey + "found prematurely in config");
    }
    config_.emplace(kRxPDOAssignmentKey, rxpdo_assignment_.get());
    config_.emplace(kTxPDOAssignmentKey, txpdo_assignment_.get());
  }

  // method to set pdo assignments
  // a pdo assignment consists of a process variable name and a pdo "address"
  // the pdo address is a string that is used to access the pdo in the rxpdo or txpdo map
  // this method should throw if the pdo address is not found in the rxpdo or txpdo map
  // for now, we'll only accept a whole assingment StringList (not individual pv and pdo address)
  void set_rxpdo_assignment(StringMap const& pv_to_pdo)
  {
    // make sure all pdo addresses are in the rxpdo map
    for (auto const& [pv, pdo] : pv_to_pdo) {
      if (rxpdo_.find(pdo) == rxpdo_.end()) {
        throw std::runtime_error("PDO address " + pdo + " not found in rxpdo map");
      }

      // also ensure no keys collide with those reserved by config or working map
      if (config_.find(pv) != config_.end()) {
        throw std::runtime_error("Key " + pv + " collides with reserved key in config map");
      }
      if (working_map_.find(pv) != working_map_.end()) {
        throw std::runtime_error("Key " + pv + " collides with reserved key in working map");
      }
    }

    // validated, now set the assignment
    active_rxpdos_.clear();
    for (auto const& [pv, pdo] : pv_to_pdo) {
      active_rxpdos_.emplace(pv, rxpdo_.at(pdo));
    }
  }

  void set_txpdo_assignment(StringMap const& pv_to_pdo)
  {
    // make sure all pdo addresses are in the rxpdo map
    for (auto const& [pv, pdo] : pv_to_pdo) {
      if (txpdo_.find(pdo) == txpdo_.end()) {
        throw std::runtime_error("PDO address " + pdo + " not found in txpdo map");
      }

      // also ensure no keys collide with those reserved by config or working map
      if (config_.find(pv) != config_.end()) {
        throw std::runtime_error("Key " + pv + " collides with reserved key in config map");
      }
      if (working_map_.find(pv) != working_map_.end()) {
        throw std::runtime_error("Key " + pv + " collides with reserved key in working map");
      }
    }

    // validated, now set the assignment
    active_txpdos_.clear();
    for (auto const& [pv, pdo] : pv_to_pdo) {
      active_txpdos_.emplace(pv, txpdo_.at(pdo));
    }
  }

  void deserialize_rxpdos(std::string const& serialized_rxpdo_map)
  {
    active_rxpdos_.deserialize(serialized_rxpdo_map);
  }

  std::string serialize_txpdos()
  {
    return active_txpdos_.serialize();
  }




};


}  // namespace sericat

#endif  // SERICAT_SLAVE_HPP
