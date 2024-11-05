
#ifndef SIMULATOR__MASTER_SIMULATOR_HPP
#define SIMULATOR__MASTER_SIMULATOR_HPP

#include "base_simulator.hpp"

namespace bb_simulator
{

class MasterSimulator : BaseSimulator{
  MasterSimulator(
    std::shared_ptr<bb_simulator::SerialLine> usb_serial_line_in,
    std::shared_ptr<bb_simulator::SerialLine> usb_serial_line_out,
    std::shared_ptr<bb_simulator::SerialLine> dc_serial_line_in,
    std::shared_ptr<bb_simulator::SerialLine> dc_serial_line_out
  ) : BaseSimulator(usb_serial_line_in, usb_serial_line_out, dc_serial_line_in, dc_serial_line_out) {}

  void setup() override {

  }

  void loop() override {

  }

};

}  // namespace bb_simulator

#endif  // SIMULATOR__MASTER_SIMULATOR_HPP