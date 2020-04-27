# eVCU
This project contains C++ code for an electric vehicle control unit (VCU) based on an Arduino. It is intended to be compliant with German TÜV requirements.

Next steps:
* Unit testing (oh oh, no test-driven development here)
* Dry testing in the CAN-bus network
* Build the electric vehicle
* Get admission
* Drive

Description follows.

# Hardware
The core modules of the eVCU are
* an Arduino Uno or Mega (smaller models do not provide enough pins),
* an MCP2515 chip with SPI for CAN-bus communication.
All other devices depend on the vehicle set-up. However, a few things are useful in every electric vehicle:
* Electric throttle (basically a potentiometer)
* Power button with LED indicating the motor status
* Emergency stop button
* Relays for switching other devices such as cooling pumps or brake lights
* User interface

# Settings
following...
* Throttle position
* Torque settings
* Pins
* CAN signal and message values
* where which CAN signal occurs
