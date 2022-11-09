# electric Vehicle Control Unit

This library allows the control of diverse components - from simple analogue switches to complex CAN-bus driven MCUs - within a vehicle or any other entity that depends on various devices. In the case of electric vehicle conversions, this flexibility allows including quality components from scrapped OEM vehicles, which saves resources and money. 

The library is based on the ESP32 microcontroller, which is easily available for small expenses. The code is fully compatible with the open source Arduino IDE and developed test-driven (unit tests found in `test/`). It follows a multi-threading Service-Oriented Architecture - event-driven and thread safe.

## Installation

This is an Arduino library and thus, available in the Arduino library manager. You need the ESP32 Arduino library, which implements all ESP32 functionalities (many tutorials available). Note, that you need the official stable release link, not the one posted in most tutorials. Official documentation: https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html

## Overview

Save and efficient operation of a vehicle requires two class types:
* `Parameter`s: Thread-safe vessels for carrying boolean, integer, and double values, respectively (`ParameterBool`, `ParameterInt`, `ParameterDouble`). Each `Parameter` has a unique ID. Examples are the throttle position, current draw, warning levels, and switch positions.
* `Device`s: Services/drivers of the vehicle that react to `Parameter` changes and cause them. Examples are the ignition switch, backlights, BMS, CAN-bus manager, and the vehicle itself.

At program start, `Device`s register their specific `Parameter`s and subscribe to value changes of other `Parameter`s, which they are interested in (by unique ID). This way, `Device`s don't know each other, and communicate solely via `Parameter`s. They execute a routine when notified of value changes. Both, reaction to value changes and observation of own inputs happens in seperate threads.

In the background operates the `VehicleController`: It registers `Parameter`s and subscriptions from `Device`s that observe their value changes. It distributes the information that a `Parameter`s value changed to the `Device`s that subscribed for it.

All hardware-specific settings - like pin definitions - go into the `Constants.h` file, whereas all other specifications and variables stay with their `Device`s, in line with the decentralised architechture.

Semaphores, queues, and threads are based on [FreeRTOS](https://www.freertos.org/) (the operating system of the ESP32). Hardware drivers like interrupts, timers, or serial communication use the [ESP32 API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html).

## Start an own project

When starting a project with this library, you must define your hardware-specific `Device`s and `Parameter`s:
1. Define `Parameter`s with unique IDs that are relevant to more than one `Device`. Instantiate them at the beginning of the main file (usually the `.ino` file).
2. Write classes for your `Device`s. See below for a programming guide and classes that are already implemented.
3. Register all `Parameter`s in the `setup()` function using the `VehicleController`s function `registerParameter()`
4. Start operation of all `Device`s by calling their `begin()` function.

Have a look at the examples for custom classes and the order of instantiation in the `.ino` file.

### Own Devices

There are some pre-defined classes that can used in your project as-is:
* `Switch`: Implements a simple I/O-switch, given a pin, a pin mode, and a `Parameter` to set.
* `Pedal`: Implements a pedal for a throttle or a brake, given an analog pin and a `ParameterDouble` for the pedal position.

Please refer to the code documentation for further details.

All custom `Device`s must inherit the library functionality from a `Device` class. There are several base classes ready for use:
* `DevicePin`: Any `Device` that uses GPIO pins with interrupts can use this base class, e.g. the `Switch`.
* `DeviceCAN`: There is one crucial device in every vehicle, which inherits from this class: the CanManager. It observes the CAN bus and sends own CAN messages based on the `Parameter`s from other `Device`s. Have a look at the DefenderAPEV528 project as an example:
* `DeviceSPI`: If there is a `Device` that uses SPI communication, it should inherit from this class, as it implements all relevant functionalities.

Further information is to be found in the corresponding code documentation.

There are some functions, which must be defined in every custom `Device`:
* `begin()`: There are three blocks to be executed in order:
	* Start the threads by calling `startTasks()` of the parent class.
	* Register for value changes of `Parameter`s by calling `this->registerForValueChanged(<Paramter ID>)`.
	* Any logic that should be executed at vehicle start (e.g. installing drivers or setting timers).
* `shutdown()`: Some parent classes require uninstallation of hardware drivers. See the code documentation.
* `onValueChanged()`: You must implement a logic for the reaction to value changee of every `Parameter` this `Device` is subscribed to (e.g. use a switch-case logic).
* Additionally, some base classes require the implementation of further functions. See the code documentation.

## Testing

The core components of this library are unit tested. Currently, there is no test framework under use. Unit testing happens with the [AUnit](https://github.com/bxparks/AUnit) library for the Arduino IDE. Just copy the required files from `vcu` into the test folder and delete them afterwards.
