# electric Vehicle Control Unit

This library allows the control of diverse components - from simple analogue switches to complex CAN-bus driven MCUs - within a vehicle or any other entity that depends on various devices. In the case of electric vehicle conversions, this flexibility allows including quality components from scrapped OEM vehicles, which saves resources and money. 

The library is based on the ESP32 microcontroller, which is easily available for small expenses. The code is fully compatible with the open source Arduino IDE and developed test-driven (unit tests found in `test/`). It follows a multi-threading Service-Oriented Architecture - event-driven and thread safe.

## Installation

This is an Arduino-style library and thus, available in the Arduino library manager (coming soon). You need the ESP32 Arduino library, which implements all ESP32 functionalities (many tutorials available). Note, that you need the official stable release link, not the one posted in most tutorials. Official documentation: https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html

## Overview

Save and efficient operation of a vehicle requires two class types:
* `Parameter`s: Thread-safe vessels for carrying boolean, integer, and double values, respectively (`ParameterBool`, `ParameterInt`, `ParameterDouble`). Each `Parameter` has a unique ID. Examples are the throttle position, current draw, warning levels, and switch positions.
* `Device`s: Services/drivers of the vehicle that react to `Parameter` changes and cause them. Examples are the ignition switch, backlights, BMS, CAN-bus manager, and the vehicle itself.

At program start, `Device`s subscribe to value changes of `Parameter`s, which they are interested in (by unique ID). This way, `Device`s don't know each other, and communicate solely via `Parameter`s. They execute a routine when notified of value changes. Both, reaction to value changes and observation of own inputs happens in seperate threads.

In the background operates the `VehicleController`: It registers `Parameter`s and subscriptions from `Device`s that observe their value changes. It distributes the information that a `Parameter`'s value changed to the `Device`s that subscribed for it.

Semaphores, queues, and threads are based on [FreeRTOS](https://www.freertos.org/) (the operating system of the ESP32). Hardware drivers like interrupts, timers, or serial communication use the [ESP32 API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html).

## Start an own project

When starting a project with this library, you must define your hardware-specific `Device`s and `Parameter`s:
1. Define `Parameter`s with unique IDs that are relevant to more than one `Device`. Best practice is to declare them `extern` in a header file (e.g. `GlobalParamteres.h`) and instantiate them in the corresponding source file (`GlobalParamteres.cpp`). You can then include them where needed.
2. Write classes for your `Device`s. See below for a programming guide and classes that are already implemented.
3. Instantiate your `Device`s in your main file (`*.ino` in Arduino-style).
4. Start operation of all `Device`s by calling their `begin()` function in the `setup()` of your program.

### Own Devices

There are some pre-defined classes that can used in your project as-is:
* `Switch`: Implements a simple I/O-switch, given a pin, a pin mode, and a `Parameter` to set.
* `Pedal`: Implements a pedal for a throttle or a brake, given an analog pin and a `ParameterDouble` for the pedal position.
* `IgnitionSwitch`: Observes three pins (KL75, KL15, KL50) to implement a standard ignition switch, writing into a `ParameterInt`.

Please refer to the code documentation for further details.

All custom `Device`s must inherit the library functionality from a `Device` class. There are several base classes ready for use:
* `DevicePin`: Any `Device` that uses one GPIO pin with interrupt can use this base class, e.g. the `Switch`.
* `DevicePinMulti`: Same functionality as `DevicePin`, but it observes multiple pins with interrupt.
* `DeviceCAN`: There is one crucial device in every vehicle, which inherits from this class: the CanManager. It observes the CAN bus and sends own CAN messages based on the `Parameter`s from other `Device`s. Have a look at the [flagship project](https://github.com/marlinarnz/eVCU_DefenderAPEV528) as an example.
* `DeviceSPI`: If there is a `Device` that uses SPI communication, it should inherit from this class, as it implements all relevant functionalities.
* `DeviceLoop`: In case polling is needed (e.g. for a pedal), this class executes a function any given time interval.

Further information is to be found in the corresponding code documentation.

There are some functions, which must be defined in every custom `Device`:
* `begin()`: There are three blocks to be executed in order:
	* Start the threads by calling `this->startTasks(<optional: stack size>)` of the parent class.
	* Register for value changes of `Parameter`s by calling `this->registerForValueChanged(<Paramter ID>)`.
	* Any logic that should be executed at vehicle start (e.g. installing drivers or setting timers).
* `shutdown()`: Some parent classes require uninstallation of hardware drivers. See the code documentation.
* `onValueChanged()`: You must implement a logic for the reaction to value changes of every `Parameter` this `Device` is subscribed to (e.g. use a switch-case logic).
* Additionally, some base classes require the implementation of further functions. See the code documentation.

## Testing

The core components of this library are unit tested. Currently, there is no test framework under use. Unit testing happens with the [AUnit](https://github.com/bxparks/AUnit) library for the Arduino IDE.
