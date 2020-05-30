/* ============================== Utility functions =======================
 * This header holds all utility funtions for communication
 */

#ifndef commUtils_h
#define commUtils_h

#include <Arduino.h>
#include <EEPROM.h>
#include "constants.h"


void report(String msg, int level, bool warn=false); //TODO
void setWarningLevel(int lvl);
void resetEEPROM();

#endif
