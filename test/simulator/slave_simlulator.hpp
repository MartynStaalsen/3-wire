
#ifndef SIMULATOR__SLAVE_SIMULATOR_HPP
#SIMULATOR__define SIMULATOR__SLAVE_SIMULATOR_HPP

#include "base_simulator.hpp"

namespace bb_simulator
{

class SlaveSimulator : BaseSimulator{
  SlaveSimulator(
    std::shared_ptr<bb_simulator::SerialLine> dc_serial_line_in,
    std::shared_ptr<bb_simulator::SerialLine> dc_serial_line_out
  ) : SlaveSimulator(nullptr, nullptr, dc_serial_line_in, dc_serial_line_out) {}

  void setup() override {

  }

  void loop() override {

  }

};

}  // namespace bb_simulator

#endif  // SIMULATOR__SLAVE_SIMULATOR_HPP