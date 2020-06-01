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
    _fadeVal(1), _prevFadeVal(1), _previousMillis(0), _lastDebounce(0)
{
}


/* ============================== Initialisation ==========================
 * Defines the button and light pin. Optionally sets the MCU state.
 * @param pin: integer button pin (must be digital)
 * @param pinLED: integer light pin (must be analog and support PWM)
 * @param MCUready: boolean MCU is ready to drive or not
 */
void PowerButton::begin(byte pin, byte pinLED, bool MCUready) {
  pinMode(pin, INPUT);      // digital pin
  pinMode(pinLED, OUTPUT);  // PWM pin
  _pin = pin;
  _pinLED = pinLED;
  _MCUready = MCUready;
  PowerButton::_setLED(42);
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
  if (_MCUready
      && (digitalRead(_pin) == HIGH)
      && (millis() - _lastDebounce > PBDD)) {
    _lastDebounce = millis();
    if (!on) {
      PowerButton::_setLED(0);
      PowerButton::_updateLED();
      PowerButton::startMotor();
      PowerButton::_setLED(2);
      report("STARTED_MOTOR", 1); // TODO
    } else {
      PowerButton::_setLED(0);
      PowerButton::_updateLED();
      PowerButton::stopMotor();
      PowerButton::_setLED(1);
      _brightnessLED = 0;
      report("STOPPED_MOTOR", 1); // TODO
    }
  }
  PowerButton::_updateLED();
}


/* ============================== Start and stop ==========================
 * Requests to start and stop the motor via the MCU
 */
void PowerButton::startMotor() {
  // Check MCU
  if (!_MCUready
      || _can->readSignal(MCU2, MCU2_WarningLevel_LSB, MCU2_WarningLevel_LEN) >= MCU2_WarningLevel_ERROR3
      || _can->readSignal(MCU2, MCU2_MotorSystemState_LSB, MCU2_MotorSystemState_LEN) == MCU2_MotorSystemState_ERROR
      || _can->checkError() != CAN_OK) {
    _MCUready = false;
    report("MOTOR_START_FAIL_DUE_MCU", 2); // TODO
  } else {
    _MCUready = true;
  }
  
  // Check OBC
  if (_can->readSignal(OBC1, OBC1_BatteryConnectStatus_LSB, OBC1_BatteryConnectStatus_LEN) == OBC1_BatteryConnectStatus_DISCONNECTED
      && int(_can->readSignal(OBC2, OBC2_ACVoltageInput_LSB, OBC2_ACVoltageInput_LEN, OBC2_ACVoltageInput_CONV_D)) == 0
      && _can->readSignal(OBC2, OBC2_SystemStatus_LSB, OBC2_SystemStatus_LEN) != OBC2_SystemStatus_CHARGE_CC
      && _can->readSignal(OBC2, OBC2_SystemStatus_LSB, OBC2_SystemStatus_LEN) != OBC2_SystemStatus_CHARGE_CV
      && int(_can->readSignal(OBC3, OBC3_ACCurrentInput_LSB, OBC3_ACCurrentInput_LEN, OBC3_ACCurrentInput_CONV_D)) == 0
      && _can->readSignal(OBC3, OBC3_OutRelayStatus_LSB, OBC3_OutRelayStatus_LSB) == OBC3_OutRelayStatus_DISCONNECTED
      && _can->readSignal(OBC3, OBC3_CCStatus_LSB, OBC3_CCStatus_LEN) == OBC3_CCStatus_DISCONNECTED
      && _can->checkError() == CAN_OK) {
    _OBCready = true;
  } else {
    _OBCready = false;
    report("MOTOR_START_FAIL_DUE_OBC", 2); // TODO
  }
  
  // Check BMS
  if (_can->checkError() != CAN_OK) {
    
    //TODO

    _BMSready = false;
    report("MOTOR_START_FAIL_DUE_BMS", 2); // TODO
  } else {
    _BMSready = true;
  }

  if (_MCUready && _BMSready && _OBCready) {
    digitalWrite(mainContactorPin, HIGH);
    digitalWrite(prechargeRelaisPin, LOW);
    _can->writeSignal(VCU1, VCU1_BMS_MainRelayCmd_LSB, VCU1_BMS_MainRelayCmd_LEN, VCU1_BMS_MainRelayCmd_ON);
    _can->writeSignal(VCU1, VCU1_BMS_AuxRelayCmd_LSB, VCU1_BMS_AuxRelayCmd_LEN, VCU1_BMS_AuxRelayCmd_OFF);
    _can->writeSignal(VCU1, VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN, VCU1_KeyPosition_CRANK);
    _can->writeSignal(VCU1, VCU1_WorkMode_LSB, VCU1_WorkMode_LEN, VCU1_WorkMode_TORQUE);
    _can->writeSignal(VCU1, VCU1_MotorMode_LSB, VCU1_MotorMode_LEN, VCU1_MotorMode_POWERDRIVE);
    _keyPosCrank = true;
  }
}

void PowerButton::stopMotor() {
  _can->writeSignal(VCU1, VCU1_MotorMode_LSB, VCU1_MotorMode_LEN, VCU1_MotorMode_STANDBY);
  _can->writeSignal(VCU1, VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN, VCU1_KeyPosition_ACC);
  
}


/* ============================== State of MCU ============================
 * Sets state of the MCU for PowerButton functionality
 * @param state: boolean if MCU is ready or not
 */
void PowerButton::setMCUready(bool state) {
  _MCUready = state;
  if (!state) {
    PowerButton::_setLED(3);
  }
}


/* ============================== State of light ==========================
 * Sets the light signal state
 * @param state: integer for states:
 *    0 "light off"; 1 "motor off"; 2 "motor on"; 3 "error"
 */
void PowerButton::_setLED(byte state) {
  _stateLED = state;
}


/* ============================== Update light pin ========================
 * Updates the light pin depending on the object's state
 */
void PowerButton::_updateLED() {
  switch (_stateLED) {
    case 0:
      digitalWrite(_pinLED, LOW);
      break;
    case 1:
      _fadeVal = (millis() - _previousMillis) * PBFF;
      if (_prevFadeVal >= 0) {
        if ((_brightnessLED + _fadeVal) < 230) {
          _brightnessLED += _fadeVal;
          _prevFadeVal = _fadeVal;
        } else {
          _brightnessLED = 230;
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
      digitalWrite(_pinLED, HIGH);
      break;
    case 3:
      if (millis() - _previousMillis >= PBBF) {
        _previousMillis = millis();
        if (_brightnessLED <= 100) {
          _brightnessLED = 255;
        } else {
          _brightnessLED = 0;
        }
        digitalWrite(_pinLED, _brightnessLED);
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
