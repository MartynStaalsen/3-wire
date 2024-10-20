// test that daisy chaining serial works.
// goal is to run this same sketch on all 4 boards (3 slaves, on master).
// thus, the master is defined as the one that has Serial avaible (daisy chaining is via Serial1)

// things to do:
// initialize Serial1 at some rate
// try to initialize Serial
// if this work

#include <HardwareSerial.h>
#include <string>

HardwareSerial ChainSerial(0);   //Create a new HardwareSerial class.


const double TARGET_HZ = 100;
#define BUFFER_SIZE 100  // number of bytes to parse (max) per slave cycle

const char query = '?';
const char terminator = '!';

bool am_master = false;

uint32_t cycle_time_ms = 0;

std::string now_millis_to_str(){
  return std::to_string(millis());
}

std::string S2s(String s){
  return std::string(s.c_str());
}

uint msg_byte_count = 0;

bool first_frame = false;

void setup() {
  cycle_time_ms = 1000.0 / TARGET_HZ; 
  // initialize both serial ports:
  Serial.begin(115200);  // usb serial
  ChainSerial.begin(115200, SERIAL_8N1, RX, TX); // at CPU Freq is 40MHz, work half speed of defined.
}



void loop() {
  if (Serial.available() && !am_master){
    am_master = true;
    Serial.println("hi there");  // DEBUG
 
  }
  // if we read anthing from usb serial, send a query down the chain
  if (am_master){;
    // send a query
    std::string send_str = "[" + now_millis_to_str() + "]?!"; 
    ChainSerial.print(send_str.c_str());

    delay(cycle_time_ms);

    // wait for return
    std::string response = S2s(ChainSerial.readStringUntil(terminator));

    // add current time to response

    // echo string to Serial
    std::string echo = response + "[" + now_millis_to_str() + "]"; 
    Serial.println(echo.c_str());
  } else {
    Serial.println("not master");  // DEBUG
    // am not master, my job is just to echo what i hear on ChainSerial
    
    //read any available bytes on input
    uint8_t read_byte_buffer[BUFFER_SIZE];
    auto available_bytes = ChainSerial.read(read_byte_buffer, BUFFER_SIZE);

    for (size_t ndx = 0; ndx < available_bytes; ndx++){
      uint8_t read_byte = read_byte_buffer[ndx];
      if (read_byte == query){
        msg_byte_count = 0;
      }
      if (read_byte == terminator){
        // add my "state"
        // don't have state right now, so instead i'll send the number of bytes i've seen since the last query character.
        // this should be uniqe per slave.
        ChainSerial.print(msg_byte_count);
        ChainSerial.print(terminator);
        first_frame = false;
      } else {
        // pass it on blindly
        ChainSerial.write(read_byte);
        msg_byte_count++;
      }
    }
  }
}
