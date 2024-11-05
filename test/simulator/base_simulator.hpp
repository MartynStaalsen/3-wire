#ifndef SIMULATOR__BASE_SIMULATOR_HPP
#define SIMULATOR__BASE_SIMULATOR_HPP

#include <atomic>
#include <deque>
#include <future>
#include <memory>
#include <iostream>
#include <string>

namespace bb_simulator
{

// class to provide inter-simulator communication
// this simulates a serial connection between two arduinos
// each line is one-directional, and contains a deque of data.
// data is written by a sender and read by a receiver.
class SerialLine {
public:
  std::deque<char> buffer;
  std::mutex mutex;

  void write(char c){
    std::lock_guard<std::mutex> lock(mutex);
    buffer.push_back(c);
  }

  void write(std::string const& s){
    std::lock_guard<std::mutex> lock(mutex);
    for(char c : s){
      buffer.push_back(c);
    }
  }

  std::string read(){
    std::lock_guard<std::mutex> lock(mutex);
    std::string s;
    while (!buffer.empty()){
      s.push_back(buffer.front());
      buffer.pop_front();
    }
    return s;
  }
};

// base class for simulating arduino
class BaseSimulator {
private:
  std::atomic<bool> running;
  virtual void setup() = 0;
  virtual void loop() = 0;

  std::shared_future<void> sim_thread_future_;

  std::shared_ptr<SerialLine> chain_serial_in_;
  std::shared_ptr<SerialLine> chain_serial_out_;
  std::shared_ptr<SerialLine> usb_serial_in_;
  std::shared_ptr<SerialLine> usb_serial_out_;

protected:

  void usb_serial_write(std::string const& s){
    usb_serial_out_->write(s);
  }

  void chain_serial_write(std::string const& s){
    chain_serial_out_->write(s);
  }

  std::string usb_serial_read(){
    return usb_serial_in_->read();
  }

  std::string chain_serial_read(){
    return chain_serial_in_->read();
  }

public:

  void spawn(){
    setup();
    std::cout << "Simulator started" << std::endl;
    while(running.load()){
      loop();
    }
    throw std::runtime_error("running went false");
  }

  BaseSimulator(
    std::shared_ptr<SerialLine> usb_serial_in,
    std::shared_ptr<SerialLine> usb_serial_out,
    std::shared_ptr<SerialLine> chain_serial_in = nullptr,
    std::shared_ptr<SerialLine> chain_serial_out = nullptr
  ) : chain_serial_in_(chain_serial_in),
      chain_serial_out_(chain_serial_out),
      usb_serial_in_(usb_serial_in),
      usb_serial_out_(usb_serial_out)
  {
    running.store(false);

    auto future = std::async(std::launch::async, &BaseSimulator::spawn, this);
    sim_thread_future_ = future.share();
  }

  ~BaseSimulator(){
    running.store(true);
  }

  std::shared_future<void> get_shared_future(){
    return sim_thread_future_;
  }

  void kill(){
    running.store(false);
  }

  bool is_running(){
    return running.load();
  }
};

}  // namespace bb_simulator

#endif  // SIMULATOR__BASE_SIMULATOR_HPP