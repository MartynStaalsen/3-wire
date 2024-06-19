# 3-wire
Python prototype of a brain-dead simplex UART protocol


# general vibe
This protocol is intended to enable daisy chaining of arduino-like microcontrollers with minimal configuration or wiring.
The application i have in mind is modular peripheral devices that add simple switches, buttons, or rotary encoders, and potentially analog devices that contribute to a macropad type of setup. My goal is to make the thing as easy to setup as possible, so no fiddling with setting an I2C address for each module, and highly generic, cheap, off-the-shelf wires and connectors for inter-module hardware.


I've landed on using stereo audio hardware (3.5mm or 1/4" depending on total current draw etc), and use only 1 channel for all data transmission.

Originally, I designed this for a 4 pin connection, but discovered that the master->slave line was really just a clock signal that is not really needed.

## design detail to follow
