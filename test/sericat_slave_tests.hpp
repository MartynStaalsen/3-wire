#ifndef SERICAT_SLAVE_TESTS_HPP_
#define SERICAT_SLAVE_TESTS_HPP_

#include "gtest/gtest.h"

#include "sericat_slave.hpp"

namespace sericat
{
  TEST(SericatSlaveTest, Construction){
    SdoMap config({
      {"1", new Sdo<bool>("config_bool", true)},
      {"2", new Sdo<std::int32_t>("config_int", -44)},
      {"3", new Sdo<float>("config_float", 3.14)},
      {"4", new Sdo<std::string>("config_string", "hi there")}
    });

    SdoMap rxpdos({
      {"1", new Sdo<bool>("rxpdo_bool")},
      {"2", new Sdo<std::int32_t>("rxpdo_int")},
      {"3", new Sdo<float>("rxpdo_float")},
      {"4", new Sdo<std::string>("rxpdo_string")}
    });

    SdoMap txpdos({
      {"1", new Sdo<bool>("txpdo_bool")},
      {"2", new Sdo<std::int32_t>("txpdo_int")},
      {"3", new Sdo<float>("txpdo_float")},
      {"4", new Sdo<std::string>("txpdo_string")}
    });

    SericatSlave slave(config, rxpdos, txpdos);

    slave.set_rxpdo_assignment({
      {"pv1", "1"},
      {"pv2", "2"},
      {"pv3", "3"},
      {"pv4", "4"}
    });

    slave.set_txpdo_assignment({
      {"hi", "1"},
      {"some_val", "2"},
    });

    EXPECT_THROW(
      slave.set_rxpdo_assignment({
        {"pv1", "1"},
        {"pv2", "2"},
        {"pv3", "3"},
        {"pv4", "5"}
      }),
      std::runtime_error
    );

    // same for txpdo

    slave.set_txpdo_assignment({
      {"txpdo_1", "2"},
      {"Txpdo2", "3"}
    });

    EXPECT_THROW(
      slave.set_txpdo_assignment({
        {"pv1", "1"},
        {"pv2", "2"},
        {"pv3", "3"},
        {"pv4", "5"}
      }),
      std::runtime_error
    );

  }

}   // namespace sericat

#endif // SERICAT_SLAVE_TESTS_HPP_
