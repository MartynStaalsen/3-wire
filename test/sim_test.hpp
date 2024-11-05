#ifndef SIM_TEST_HPP
#define SIM_TEST_HPP

#include "gtest/gtest.h"

#include "helper_header.hpp"
#include <chrono>
#include <thread>

#include "base_simulator.hpp"

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