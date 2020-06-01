/* ============================== Utility functions ======================= 
 * This header holds all utility funtions for driving including the throttle
 * class
 */

#ifndef driveUtils_h
#define driveUtils_h

#include "Arduino.h"
#include "constants.h"
#include <EEPROM.h>

void cool(int motorTemp, int controllerTemp);
void updateRecu();
void updateDirec();

#endif
