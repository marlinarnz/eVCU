/** @file
 *  Saves all config data.
 *  A place for the pin layout, global settings, macros, and constants.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// Development settings
#define DEBUG 1


/*
 * Pin layout
 */
#define GPIO_MOSI 12
#define GPIO_MISO 13
#define GPIO_CS 14
#define GPIO_SCLK 15


/*
 * Logging and situation handling
 */
#define START_SERIAL Serial.begin(9600);
#define PRINT(msg) Serial.println(msg); //TODO
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/log.html
#define RESET ESP.restart() // For ESP32
// For Arduino Due: rstc_start_software_reset(RSTC)

/*
 * Global constants
 */
#define SWITCH_DEBOUNCE_MS 400
#define QUEUE_SIZE 20

#endif
