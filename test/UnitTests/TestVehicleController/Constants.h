/* ======================================================================
 * Central location for saving microcontroller-specific constants such
 * as I/O pins.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// Development settings
#define DEBUG 1
#define START_SERIAL Serial.begin(9600);
#define PRINT(msg) Serial.println(msg);

#define RESET ESP.restart() // For ESP32
// For Arduino Due: rstc_start_software_reset(RSTC)

// Size of an event queue
#define QUEUE_SIZE 20

#endif
