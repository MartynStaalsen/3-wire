#include "MyClass.hpp"

MyClass myObject;

void setup() {
    Serial.begin(115200);
    Serial.write("Setup complete\n");

    // watch for incoming bytes
    auto input_array = new byte[10];
}

bool blocking_read(byte* bytes, int len=1, int timeout_ms = 1000) {
    auto start_time = millis();
    while (Serial.available() == 0) {
        // wait for incoming bytes
        if (millis() - start_time > timeout_ms) {
            return false;
        }
    }
    for (int i = 0; i < len; i++) {
        bytes[i] = Serial.read();
    }
    return true;
}

void loop() {
    // read incoming bytes over serial in doublets. First is an address, second is a value
    if (Serial.available() >= 2) {
        byte address = Serial.read();
        byte value = Serial.read();

        input_array[address] = value;
    }
    
}