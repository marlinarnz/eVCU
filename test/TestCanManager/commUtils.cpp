/* ============================== Utility functions ======================= 
 * This file defines utility funtions for communication
 */

#include "commUtils.h"


/* ============================== Report messages =========================
 * Transmits messages to the UI using a CAN frame
 * @param msg: integer message ID
 * @param level: integer 0 = "debug"; 1 = "info"; 2 = "warn"; 3 = "error"
 * @param warn: (optional) boolean whether to set the VCU warning level
 */
void report(String msg, int level, bool warn) {  //TODO
  if (!Serial.available()) {
    Serial.begin(9600);         // Initialize the USB serial
    while (!Serial) {;}         // Wait until it started. Only for native port
  }
  if (level <= 3 && level >= 0) {
    Serial.println(msg);
  
    //TODO fill bytes of CAN frame VCUMSG with id of msg (6 bits) and level (2 bits)
    // results in 63 possible messages
    //
    // Report the loop time on debug level
    //String loopTime = String(millis() - lastLoop);
    //report("Loop time (ms): " + loopTime, 0);
  
    if (warn) {
      setWarningLevel(level);
    }
  }
}


/* ============================== Warning level ===========================
 * Sets the warning level of the VCU to a valid level (0 to 3)
 * @param lvl: integer 0 = "OK"; 1 = "low"; 2 = "medium"; 3 = "high"
 */
void setWarningLevel(int lvl) {
  if (lvl <= 3 && lvl >= 0) {
    EEPROM.put(VCU1_WARNING_LEVEL, lvl);
  }
}


/* ============================== EEPROM ==================================
 * The resetEEPROM() function must be run once in the initial setup.
 * Afterwards, it resets all values back to default.
 * NOTE: There is a limit of 100,000 write cycles per single location.
 * Therefore, using put() is preferred as it includes the update() function
 */
void resetEEPROM() {
  EEPROM.put(VCU1_WARNING_LEVEL, vcu1WarningLevel);
  EEPROM.put(COOL_THRESHOLD, coolThreshold);
  EEPROM.put(MAX_TORQUE, maxTorque);
  EEPROM.put(MAX_NEG_TORQUE, maxNegTorque);
  EEPROM.put(MAX_RPM, maxRPM);
  EEPROM.put(BRAKE_LIGHT_TORQUE_THRESHOLD, brakeLightTorqueThreshold);
  EEPROM.put(THROTTLE_POS_MAX_TORQUE, throttlePosMaxTorque);
  EEPROM.put(THROTTLE_POS_HALF_TORQUE, throttlePosHalfTorque);
  EEPROM.put(THROTTLE_POS_ZERO_TORQUE, throttlePosZeroTorque);
  EEPROM.put(THROTTLE_POS_ZERO_NEG_TORQUE, throttlePosZeroNegTorque);
  EEPROM.put(THROTTLE_POS_MAX_NEG_TORQUE, throttlePosMaxNegTorque);
  EEPROM.put(THROTTLE_POS_START, throttlePosStart);
  EEPROM.put(BRAKE_PEDAL_FUNCTION, brakePedalFunction);
}
