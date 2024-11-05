#ifndef SIMULATOR__SIM_TEST_HPP
#define SIMULATOR__SIM_TEST_HPP

#include "gtest/gtest.h"

#include <chrono>
#include <thread>

#include "base_simulator.hpp"
#include "sericat_sdo_map.hpp"

namespace sim_test
{
class HelloSimulator : public bb_simulator::BaseSimulator {
private:
  int count = 0;

  void setup() override {
    usb_serial_write("Hello from slave");
  }

  void loop() override {
    std::cout << "HelloSimulator loop" << std::endl;

    count++;
    std::string s = usb_serial_read();
    if (s == "Hello from master"){
      usb_serial_write("Hello from slave");
    }
  }
public:
  HelloSimulator(
    std::shared_ptr<bb_simulator::SerialLine> usb_serial_in,
    std::shared_ptr<bb_simulator::SerialLine> usb_serial_out
  ) : BaseSimulator(usb_serial_in, usb_serial_out)
  {}
};

// like a slave, but just has a couple sdos (no pdos yet)
class SDOHaver : public bb_simulator::BaseSimulator {
private:
  sericat::Sdo<bool> sdo_bool_;
  sericat::Sdo<std::int32_t> sdo_int_;
  sericat::Sdo<float> sdo_float_;
  sericat::Sdo<std::string> sdo_string_;

  sericat::SdoMap local_map_;


  void setup() override {
    // do nothing for now
  }

  void loop() override {
    // TODO: add querying
  }
public:
  // has no usb, only chain
  SDOHaver(
    std::shared_ptr<bb_simulator::SerialLine> chain_serial_in,
    std::shared_ptr<bb_simulator::SerialLine> chain_serial_out,
    bool initial_bool,
    std::int32_t initial_int,
    float initial_float,
    std::string initial_string
  ) : BaseSimulator(nullptr, nullptr, chain_serial_in, chain_serial_out),
      sdo_bool_("some_bool", initial_bool),
      sdo_int_("some_int", initial_int),
      sdo_float_("some_float", initial_float),
      sdo_string_("some_string", initial_string),
      local_map_({
        {"Bob", &sdo_bool_},
        {"Irma", &sdo_int_},
        {"Frank", &sdo_float_},
        {"Sue", &sdo_string_}
      })
  {}
};


TEST(SimTest, HelloSimulator){
  auto usb_serial_in = std::make_shared<bb_simulator::SerialLine>();
  auto usb_serial_out = std::make_shared<bb_simulator::SerialLine>();

  HelloSimulator slave(usb_serial_out, usb_serial_in);

  usb_serial_out->write("Hello from master");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::string s = usb_serial_in->read();
  EXPECT_EQ(s, "Hello from slave");
}

}  // namespace sim_test

#endif  // SIM_TEST_HPP