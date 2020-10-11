/* ============================== Power button ============================
 * The PowerButton starts and stops the vehicle and signals its motor status
 * with a light.
 */

#include "PowerButton.h"

extern bool on;
extern unsigned long prechargeStart;

/* ============================== Constructor =============================
 * Instantiates a PowerButton object with its private attributes
 * @param can: CanManager object pointer
 */
PowerButton::PowerButton(CanManager* can)
    : _can(can), _pin(0), _pinLED(),
    _MCUready(false), _OBCready(false), _BMSready(false),
    _keyPosCrank(false), _stateLED(0), _brightnessLED(0),
    _fadeVal(1), _prevFadeVal(1), _previousMillis(0), _lastDebounce(0) {
}
PowerButton::PowerButton()
    : _can(NULL), _pin(0), _pinLED(),
    _MCUready(false), _OBCready(false), _BMSready(false),
    _keyPosCrank(false), _stateLED(0), _brightnessLED(0),
    _fadeVal(1), _prevFadeVal(1), _previousMillis(0), _lastDebounce(0) {
}


/* ============================== Initialisation ==========================
 * Defines the button and light pin. Optionally sets the MCU state.
 * @param pin: integer button pin (must be digital)
 * @param pinLED: integer light pin (must be analog and support PWM)
 * @param MCUready: boolean MCU is ready to drive or not
 */
void PowerButton::begin(byte pin, byte pinLED) {
  pinMode(pin, INPUT);      // digital pin
  pinMode(pinLED, OUTPUT);  // PWM pin
  _pin = pin;
  _pinLED = pinLED;
  PowerButton::_setLED(!on);
  PowerButton::_updateLED();
}


/* ============================== Update ==================================
 * Updates the button state if debounce delay is over and starts or stops
 * the motor. Does not react when MCU is not ready. Also updates the
 * light pin.
 */
void PowerButton::update() {
  if (_keyPosCrank && millis() - _lastDebounce > PBDD) {
    _can->writeSignal(VCU1, VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN, VCU1_KeyPosition_ON);
    _keyPosCrank = false;
  }
  if ((digitalRead(_pin) == HIGH)
      && (millis() - _lastDebounce > PBDD)) {
    _lastDebounce = millis();
    if (!on) {
      PowerButton::_setLED(0);
      PowerButton::_updateLED();
      PowerButton::startMotor();
    } else {
      PowerButton::_setLED(0);
      PowerButton::_updateLED();
      PowerButton::stopMotor();
    }
  }
  if (!_MCUready) {
    PowerButton::_checkMCUready();
  }
  if (!_BMSready) {
    PowerButton::_checkBMSready();
  }
  if (_OBCready) {
    PowerButton::_checkOBCready();
  }
  PowerButton::_updateLED();
}


/* ============================== Start and stop ==========================
 * Requests to start and stop the motor via the MCU
 */
void PowerButton::startMotor() {
  // Check devices
  PowerButton::_checkMCUready();
  PowerButton::_checkBMSready();
  PowerButton::_checkOBCready();

  if (_MCUready && _BMSready && !_OBCready) {
    digitalWrite(mainContactorPin, HIGH);
    digitalWrite(prechargeRelaisPin, LOW);
    _can->writeSignal(VCU1, VCU1_BMS_MainRelayCmd_LSB, VCU1_BMS_MainRelayCmd_LEN, VCU1_BMS_MainRelayCmd_ON);
    _can->writeSignal(VCU1, VCU1_BMS_AuxRelayCmd_LSB, VCU1_BMS_AuxRelayCmd_LEN, VCU1_BMS_AuxRelayCmd_OFF);
    _can->writeSignal(VCU1, VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN, VCU1_KeyPosition_CRANK);
    _can->writeSignal(VCU1, VCU1_WorkMode_LSB, VCU1_WorkMode_LEN, VCU1_WorkMode_TORQUE);
    _can->writeSignal(VCU1, VCU1_MotorMode_LSB, VCU1_MotorMode_LEN, VCU1_MotorMode_POWERDRIVE);
    _keyPosCrank = true;
    PowerButton::_setLED(2);
    report("STARTED_MOTOR", 1); // TODO
  } else {
    report("MOTOR_START_FAULT", 3); //TODO
  }
}

void PowerButton::stopMotor() {
  _can->writeSignal(VCU1, VCU1_MotorMode_LSB, VCU1_MotorMode_LEN, VCU1_MotorMode_STANDBY);
  _can->writeSignal(VCU1, VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN, VCU1_KeyPosition_ACC);
  PowerButton::_setLED(1);
  report("STOPPED_MOTOR", 1); // TODO
  // Check devices
  PowerButton::_checkMCUready();
  PowerButton::_checkBMSready();
  PowerButton::_checkOBCready();
}


/* ============================== State of MCU ============================
 * Checks state of the MCU for PowerButton functionality and saves it
 */
void PowerButton::_checkMCUready() {
  _MCUready = _can->checkMCUready(true);
  if (!_MCUready) {
    PowerButton::_setLED(3);
  }
}


/* ============================== State of BMS ============================
 * Checks state of the BMS for PowerButton functionality and saves it
 */
void PowerButton::_checkBMSready() {
  _BMSready = _can->checkBMSready(true);
  if (!_BMSready) {
    PowerButton::_setLED(3);
  }
}


/* ============================== State of OBC ============================
 * Checks state of the OBC for PowerButton functionality and saves it
 * NOTE: The car should not start when the OBC is working
 */
void PowerButton::_checkOBCready() {
  _OBCready = (_can->readSignal(OBC1, OBC1_StartStatus_LSB, OBC1_StartStatus_LEN) == OBC1_StartStatus_ON)
              && (_can->checkError() == CAN_OK);
  if (_OBCready) {
    PowerButton::_setLED(0);
  }
}


/* ============================== State of light ==========================
 * Sets the light signal state
 * @param state: integer for states:
 *    0 "light off"; 1 "motor off"; 2 "motor on"; 3 "error"
 */
void PowerButton::_setLED(byte state) {
  _stateLED = state;
  if (state == 1) {             // Start settings for the fading
    _brightnessLED = 0;
    _prevFadeVal = - 1;
  }
}


/* ============================== Update light pin ========================
 * Updates the light pin depending on the object's state
 */
void PowerButton::_updateLED() {
  switch (_stateLED) {
    case 0:
      analogWrite(_pinLED, 0);
      break;
    case 1:
      _fadeVal = (millis() - _previousMillis) * PBFF; // Fading slope
      if (_brightnessLED < 25) {                      // Adjustment
        _fadeVal *= 0.3;
      }
      if (_prevFadeVal >= 0) {
        if ((_brightnessLED + _fadeVal) < 100) {
          _brightnessLED += _fadeVal;
          _prevFadeVal = _fadeVal;
        } else {
          _brightnessLED = 100;
          _prevFadeVal = - 1;
        }
      } else {
        if ((_brightnessLED - _fadeVal) > 0) {
          _brightnessLED -= _fadeVal;
          _prevFadeVal = - _fadeVal;
        } else {
          _brightnessLED = 0;
          _prevFadeVal = 1;
        }
      }
      _previousMillis = millis();
      analogWrite(_pinLED, int(_brightnessLED));
      break;
    case 2:
      analogWrite(_pinLED, 255);
      break;
    case 3:
      if (millis() - _previousMillis >= PBBF) {
        _previousMillis = millis();
        if (_brightnessLED <= 100) {
          _brightnessLED = 255;
        } else {
          _brightnessLED = 0;
        }
        analogWrite(_pinLED, int(_brightnessLED));
      }
      break;
    default:
      if (on) {
        PowerButton::_setLED(2);
      } else {
        PowerButton::_setLED(1);
      }
      PowerButton::_updateLED();
      break;
  }
}
