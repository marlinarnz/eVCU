/* ============================== Utility functions ======================= 
 * This file defines utility funtions for driving
 */

#include "driveUtils.h"

extern bool recu;
extern bool direc;


/* ============================== Cooling control =========================
 * Check the temperatures of drivetrain devices and switch on the
 * corresponding cooling cycle
 * @param motorTemp: integer motor temperature
 * @param controllerTemp: integer controller temperature
 */
void cool(int motorTemp, int controllerTemp) {
  int threshold = EEPROM.read(COOL_THRESHOLD);
  if (motorTemp >= threshold || controllerTemp >= threshold) {
    digitalWrite(coolantCyclePin, HIGH);
  } else {
    digitalWrite(coolantCyclePin, LOW);
  }
}


/* ============================== Direction state =========================
 * Checks the state of the drive direction switch and saves it into a variable.
 * Switch is necessary for the German TÜV but does make no sense.
 * NOTE: Pin is low means forward driving
 */
void updateDirec() {
  if (digitalRead(direcPin) == LOW) {
    direc = true;
  } else {
    direc = false;
  }
}


/* ============================== Recuperation state ======================
 * Checks the state of the brake energy recuperation switch
 * NOTE: Pin is high leads to brake energy recuperation
 */
void updateRecu() {
  if (digitalRead(recuPin) == LOW) {
    recu = false;
  } else {
    recu = false;
  }
}
