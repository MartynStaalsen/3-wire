#ifndef SERICAT_SLAVE_HPP
#define SERICAT_SLAVE_HPP

#include "sericat_sdo_map.hpp"

namespace sericat
{

class SericatSlave {
private:
  SdoMap config_;
  SdoMap rxpdo_;
  SdoMap txpdo_;

  std::shared_ptr<Sdo<StringList>> rxpdo_assignment_;  // for reporting purposes
  std::shared_ptr<Sdo<StringList>> txpdo_assignment_;

  SdoMap active_rxpdos_;
  SdoMap active_txpdos_;

public:
  // constructor
  // at construct time, contents (but not data) of
  // config, rxpdo, and txpdo maps must be set

  static const inline std::string kRxPDOAssignmentKey = "RxPDO_ASSIGN";
  static const inline std::string kTxPDOAssignmentKey = "TxPDO_ASSIGN";

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
    }

    // validated, now set the assignment
    active_txpdos_.clear();
    for (auto const& [pv, pdo] : pv_to_pdo) {
      active_txpdos_.emplace(pv, txpdo_.at(pdo));
    }
  }

  void deserialize_rxpdos(std::string const& serialized_rxpdo_map)
  {
    active_rxpdos_.deserialize(serialized_rxpdo_map); // TODO: need to convert from process names to my names
  }

  std::string serialize_txpdos()
  {
    return active_txpdos_.serialize();
  }



};


}  // namespace sericat

#endif  // SERICAT_SLAVE_HPP
