# eVCU
This project contains C++ code for an electric vehicle control unit (eVCU) based on an Arduino. It is intended to be compliant with German TÜV requirements.
The advantage of this code against other project is the possibility to communicate with every possible ECU via CAN-bus. It is currently adjusted to the set of CAN messages from the AlphaElectrics APEV528 PMSM controller but can easily be extendet to other CAN signals.

Next steps:
* Component testing on the working CAN-bus
* Build the electric vehicle
* Get admission
* Drive

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
