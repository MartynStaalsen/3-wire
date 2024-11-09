#ifndef SERICAT_SLAVE_TESTS_HPP_
#define SERICAT_SLAVE_TESTS_HPP_

#include "gtest/gtest.h"

#include "sericat_slave.hpp"

#include <iostream>

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

TEST(SericatSlaveTest, SerDeser){
  Sdo<bool> some_local_bool("some_bool", true);
  Sdo<std::int32_t> some_local_int("some_int", 123);
  Sdo<float> some_local_float("some_float", 123.456f);
  Sdo<std::string> some_local_string("some_string", "Hello, World!");

  Sdo<bool> some_received_bool("some_rx_bool");
  Sdo<std::int32_t> some_received_int("some_rx_int");
  Sdo<float> some_received_float("some_rx_float");
  Sdo<std::string> some_received_string("some_rx_string");

  SdoMap local_rxpdos({
    {"rx_1", &some_received_bool},
    {"rx_2", &some_received_int},
    {"rx_3", &some_received_float},
    {"rx_4", &some_received_string}
  });

  SdoMap local_txpdos({
    {"tx_1", &some_local_bool},
    {"tx_2", &some_local_int},
    {"tx_3", &some_local_float},
    {"tx_4", &some_local_string}
  });

  SericatSlave slave({}, local_rxpdos, local_txpdos);

  slave.set_rxpdo_assignment({
    {"pv_1", "rx_1"},
    {"pv_2", "rx_2"},
    {"pv_3", "rx_3"},
    {"pv_4", "rx_4"}
  });

  slave.set_txpdo_assignment({
    {"hi", "tx_1"},
    {"some_val", "tx_2"},
  });


  // remote instance of the pdos
  SdoMap remote_rxpdos({
    {"pv1", std::make_shared<Sdo<bool>>("Bob").get()},
    {"pv2", std::make_shared<Sdo<std::int32_t>>("Irma").get()},
    {"pv3", std::make_shared<Sdo<float>>("Frank").get()},
    {"pv4", std::make_shared<Sdo<std::string>>("Sue").get()}
  });

  SdoMap remote_txpdos({
    {"hi", std::make_shared<Sdo<bool>>("Todd").get()},
    {"some_val", std::make_shared<Sdo<std::int32_t>>("Tracie").get()}
  });

  set(*remote_rxpdos["pv1"], false);
  set(*remote_rxpdos["pv2"], 456);
  set(*remote_rxpdos["pv3"], 456.789f);
  set(*remote_rxpdos["pv4"], std::string("Goodbye, World!"));

  set(*local_txpdos["tx_1"], true);
  set(*local_txpdos["tx_2"], 123);

  std::cout << "serializing rxpdos:" << std::endl;
  for (auto const& [key, sdo] : remote_rxpdos) {
    std::cout << " " << key << " " << std::endl;
  }
  std::cout << std::endl;

  std::string serialized_rxpdos = remote_rxpdos.serialize();
  std::cout << "deserializing rxpdos" << std::endl;
  slave.deserialize_rxpdos(serialized_rxpdos);

  std::cout << "deserializing txpdos" << std::endl;
  remote_txpdos.deserialize(slave.serialize_txpdos());

  EXPECT_EQ(some_received_bool.get(), false);
  EXPECT_EQ(some_received_int.get(), 456);
  EXPECT_EQ(some_received_float.get(), 456.789f);
  EXPECT_EQ(some_received_string.get(), "Goodbye, World!");

  EXPECT_EQ(get<bool>(*local_txpdos["tx_1"]), true);
  EXPECT_EQ(get<int32_t>(*local_txpdos["tx_2"]), 123);
}


}   // namespace sericat

#endif // SERICAT_SLAVE_TESTS_HPP_
