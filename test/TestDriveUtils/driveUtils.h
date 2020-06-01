/* ============================== Utility functions ======================= 
 * This header holds all utility funtions for driving
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
