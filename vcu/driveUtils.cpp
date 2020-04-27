/* ============================== Utility functions ======================= 
 * This file defines utility funtions for driving and the throttle class
 */

#include "driveUtils.h"
#include <EEPROM.h>

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
 * NOTE: Pin is low leads to brake energy recuperation
 */
void updateRecu() {
  if (digitalRead(recuPin) == LOW) {
    recu = true;
  } else {
    recu = false;
  }
}


/* ============================== Throttle Constructor ====================
 * Instantiates the throttle object
 * @param canManager: CanManager class object pointer
 */
Throttle::Throttle(CanManager* canManager) {
  byte _throttlePin;
  byte _brakePedPin;
  bool _brakePedalFunction;
  float _prevVals[TTSF];
  CanManager* _can = canManager;
}


/* ============================== Throttle Init ===========================
 * Initialises pins of the throttle object and checks the brake pedal setting
 * @param tPin: integer analog pin for throttle voltage
 * @param bPin: (optional) integer analog pin for break pedal voltage
 */
void Throttle::begin(int tPin, int bPin) {
  pinMode(tPin, INPUT);
  _throttlePin = tPin;
  _brakePedPin = bPin;
  _brakePedalFunction = EEPROM.read(BRAKE_PEDAL_FUNCTION);
  if (bPin != 255) {
    pinMode(bPin, INPUT);
    if (!_brakePedalFunction) {
      report("BRAKE_PEDAL_WRONG_SETTINGS", 1);  //TODO
    }
  } else if (_brakePedalFunction) {
    report("BRAKE_PEDAL_WRONG_SETTINGS", 3, true);  //TODO
  }
  // Initialise previous torque request array
  for(byte i=0; i<TTSF; i++) {
    _prevVals[i] = 0;
  }
}


/* ============================== Throttle reset ==========================
 * Resets the throttle position to 0
 */
void Throttle::reset() {
  byte zero = 0;
  for(byte i=0; i<TTSF; i++) {
    _prevVals[i] = zero;
  }
}


/* ============================== Throttle update =========================
 * Reads throttle and break pedal position (if existent) and writes all
 * necessary information into the corresponding messages of the CanManager.
 * Light-up of brake lights does the warn signal manager.
 * TODO: integrate cruise control
 */
void Throttle::update() {  
  // Set direction depending on the direction variable
  //TODO: test
  if (direc) {
    _can->writeSignal(VCU1, VCU1_GearLeverPosSts_LSB, VCU1_GearLeverPosSts_LEN, VCU1_GearLeverPosSts_D);
  } else {
    _can->writeSignal(VCU1, VCU1_GearLeverPosSts_LSB, VCU1_GearLeverPosSts_LEN, VCU1_GearLeverPosSts_R);
  }

  // Read the pedals
  uint8_t mode = VCU1_MotorMode_POWERDRIVE;
  uint8_t brakePedSts = VCU1_BrakePedalSts_FREE;
  float torqueReq = Throttle::_translateThrottleToTorque();
  if (_brakePedalFunction && recu) {
    float breakPedReq = - Throttle::_translateBreakPedToTorque();
    if (breakPedReq > 0) {
      if (torqueReq > 0) {
        torqueReq = torqueReq - breakPedReq;      // Reduce throttle torque request
      } else {
        torqueReq = min(torqueReq, breakPedReq);  // Take larger neg. torque request
      }
      brakePedSts = VCU1_BrakePedalSts_PRESSED;
    }
  }
  
  // Check if max RPM is exceeded
  int actSpd = _can->readSignal(MCU1, MCU1_ActMotorSpd_LSB, MCU1_ActMotorSpd_LEN, MCU1_ActMotorSpd_CONV_D);
  if (actSpd > (EEPROM.read(MAX_RPM) * 1000)
      && actSpd != MCU1_ActMotorSpd_INVALID) {
    torqueReq = min(0, torqueReq);
  }
  
  // Smooth out the torque request
  torqueReq = Throttle::_smoothTorqueReq(torqueReq);
  
  // Truncate
  if (torqueReq < 0 && recu) {
    mode = VCU1_MotorMode_GENERATE;
    torqueReq = abs(torqueReq);
  } else {
    torqueReq = max(0, torqueReq);
  }

  // Write it down
  _can->writeSignal(VCU1, VCU1_MotorMode_LSB, VCU1_MotorMode_LEN, mode);
  _can->writeSignal(VCU1, VCU1_MotorMode_LSB, VCU1_MotorMode_LEN, brakePedSts);
  // Write torque request in a range of 0 to 255, which makes the conversion
  // factor obsolete
  _can->writeSignal(VCU1, VCU1_TorqueReq_LSB, VCU1_TorqueReq_LEN, int(min(255, torqueReq)));
}


/* ============================== Smoothe request =========================
 * Smoothes out the torque request using its previous values
 * @param torque: float positive or negative torque
 * @return: float positive or negative torque smoothed
 */
float Throttle::_smoothTorqueReq(float torque) {
  float sum;
  for(byte i=0; i<TTSF; i++) {
    sum += _prevVals[i];
  }
  float torqueSmooth = (torque + sum) / (TTSF + 1);
  // Update _prevVals
  for(byte i=0; i<TTSF; i++) {
    if (i == TTSF - 1) {
      _prevVals[i] = torqueSmooth;
    } else {
      _prevVals[i] = _prevVals[i + 1];
    }
  }
  return torqueSmooth;
}


/* ============================== Throttle position =======================
 * Translates the throttle position (read voltage from analog pin) into
 * the torque request for the controller
 * @return: float positive or negative torque
 */
float Throttle::_translateThrottleToTorque() {
  // Read throttle position into a range of 0 to 100%
  float throttlePos = analogRead(throttlePin) / 10.23;
  // Translate to torque using throttle curve points from the settings
  float torque = 0;
  int tqMax = EEPROM.read(MAX_TORQUE);
  int tqNegMax = EEPROM.read(MAX_NEG_TORQUE);
  int posStart = EEPROM.read(THROTTLE_POS_START);
  int posNegMax = EEPROM.read(THROTTLE_POS_MAX_NEG_TORQUE);
  int posNegZero = EEPROM.read(THROTTLE_POS_ZERO_NEG_TORQUE);
  int posZero = EEPROM.read(THROTTLE_POS_ZERO_TORQUE);
  int posHalf = EEPROM.read(THROTTLE_POS_HALF_TORQUE);
  int posMax = EEPROM.read(THROTTLE_POS_MAX_TORQUE);
  if (throttlePos >= posStart) {
    if (posNegMax > posStart) {
      torque = - (((throttlePos-posStart) / (posNegMax-posStart))
              * tqNegMax);
    }
    if (throttlePos >= posNegMax) {
      if (posNegZero > posNegMax) {
        torque = - ( (1 - (throttlePos-posNegMax) / (posNegZero-posNegMax))
                * tqNegMax);
      }
      if (throttlePos >= posNegZero) {
        torque = 0;
        if (throttlePos >= posZero) {
          if (posHalf > posZero) {
            torque = ((throttlePos-posZero) / (posHalf-posZero))
                    * tqMax/2;
          }
          if (throttlePos >= posHalf) {
            if (posMax > posHalf) {
              torque = tqMax/2 + ((throttlePos-posHalf) / (posMax-posHalf))
                      * tqMax/2;
            }
            if (throttlePos >= posMax) {
              torque = tqMax;
            }
          }
        }
      }
    }
  }
  return torque;
}


/* ============================== Break pedal position ====================
 * Translates the brake pedal position (read voltage from analog pin) into
 * the negative torque request for the controller
 * @return: unsigned float negative torque
 */
float Throttle::_translateBreakPedToTorque() {
  // Read break pedal position into a range of 0 to 100%
  float brakePedPos = analogRead(brakePedalPin) / 10.23;
  // Cut off lower and upper ends to prevent irritations
  if (brakePedPos < BPLC) {
    return 0;
  } else {
    brakePedPos = min(BPUC, brakePedPos);
    return brakePedPos / 100 * EEPROM.read(MAX_NEG_TORQUE);
  }
}
