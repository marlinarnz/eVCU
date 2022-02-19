# electric Vehicle Control Unit

This project allows the control of diverse components - from simple analogue switches to complex CAN-bus driven MCUs - within an (electric) vehicle conversion. This flexibility allows including quality components from scrapped OEM vehicles, which saves resources and money. It is based on the ESP32 microcontroller, which is easily available for small expenses. The code is fully compatible with the open source Arduino IDE and developed test-driven. It follows a Service-Oriented Architecture - event-driven and thread safe.

# The code

## Repository structure

The `vcu` folder contains code that goes onto the microcontroller, while unit and component tests are to be found in `test`. The `main` branch contains base classes only, whereas specific conversion projects have their own branches with corresponding names.

## Inner workings

The code consists of four main classes:
- `Parameter`s: Thread-safe vessels for carrying boolean, integer, and double values, respectively. Each parameter has a unique ID. Examples are the throttle position, current draw, warning levels, and switch positions.
- `Device`s: Services of the vehicle that react to parameter changes and observe their input values. Examples are the ignition switch, backlights, BMS, CAN-bus manager, and the vehicle itself.
- `VehicleController`: Registers `Parameter`s and subscribtions to their value changes. Notifies `Device`s in order to start their routines.
- `MapParameterToDevices`: A thread-safe dynamic map of `Parameter` ID's with a corresponding list of devices that subscribed to this `Parameter`'s changes.

`Device`s register their specific `Parameter`s and subscribe to value changes of other `Device`'s `Parameter`s (by unique ID). They execute a routine when notified by the `VehicleController`. In a parallel task, they observe their input values and notify the `VehicleController` when changes occur. A central device is the CanManager, which observes the CAN-bus and sends own CAN messages based on the information from other `Device`s.

All hardware-specific settings - like pin definitions - go into the `Constants.h` file, whereas all other specifications and variables stay with their `Device`s.

Semaphores, queues, and tasks are based on [FreeRTOS](https://www.freertos.org/) (the operating system of the ESP32). This could be modified for other microcontrollers like the Arduino Due in theory.

## New Devices

Defining and relating new `Device`s in a conversion project is relatively simple (see examples in other projects/branches):
1. Define globally relevant `Parameter`s with unique IDs
2. Write code for input observation via ESP32 pins and notifications when `Parameter`s changed
3. Write a `switch`-`case` logic for value changes of other `Parameter`s
4. Fill in logic for the `init()`, `run()`, and `shutdown()` methods
5. Instantiate the `Device` in the `setup()` of the `vcu.ino` main file

## Testing

Currently, there is no test framework under use. Unit testing happens with the [AUnit](https://github.com/bxparks/AUnit) library for the Arduino IDE. Just copy the required files from `vcu` into the test folder and delete them afterwards.

Component tests are a mix of tests for external components (MCU, OBC) of the corresponding project and more complex test cases.

# The DefenderAPEV528 project

