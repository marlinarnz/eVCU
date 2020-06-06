#line 2 "TestThrottle.ino"

#include <AUnitVerbose.h>
#include "constants.h"
#include "CanManager.h"
#include "commUtils.h"
#include "Throttle.h"
#include <EEPROM.h>

using aunit::TestRunner;

bool recu = true;
bool direc = true;

// RPM that the throttle reads
float motorRpm = 2000;

// Values that the throttle writes
int motorMode = 0;
int brakePedStatus = 0;
int torqueRequest = 0;
int directionRequest = 0;

// Values to test
int maxTorqueReq = int(maxTorque);
int maxNegTorqueReq = int(maxNegTorque);
int minTorqueReq = 0;
int brakePedPressed = VCU1_BrakePedalSts_PRESSED;

CanManager mock;
Throttle classUnderTest(&mock);

// Prepare the serial port
void setup() {
    Serial.begin(9600);
    TestRunner::setTimeout(240);
    resetEEPROM(); // only necessary the first time running the tests
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

/*test(begin_noBrakePinBrakeOn_fail) {
  EEPROM.put(BRAKE_PEDAL_FUNCTION, 1);
  classUnderTest.begin(throttlePin);
  int lvl = 3;
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL), lvl);
  setWarningLevel(0);
  EEPROM.put(BRAKE_PEDAL_FUNCTION, 0);
}*/

test(begin_brakePinBrakeOff_success) {
  classUnderTest.begin(throttlePin, brakePedalPin);
  int lvl = 0;
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL), lvl);
}

test(begin_noBrakePinBrakeOff_success) {
  classUnderTest.begin(throttlePin);
  int lvl = 0;
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL), lvl);
}

test(update_recuOff_success) {
  Serial.println("Start test update_recuOff_success. Enter 'y' to approve");
  recu = false;
  char rx;
  int prevTorqueReq = 0;
  float variance = 1;
  while(rx != 'y') {
    classUnderTest.update();
    if (torqueRequest < prevTorqueReq - variance || torqueRequest > prevTorqueReq + variance) {
      Serial.print("Torque request changed to " + String(torqueRequest));
      prevTorqueReq = torqueRequest;
      if (motorMode == VCU1_MotorMode_POWERDRIVE){
        Serial.println("; Motor mode power drive");
        assertTrue((torqueRequest <= maxTorqueReq) && (torqueRequest >= minTorqueReq));
      } else {
        Serial.println("; Motor mode generate");
        recu = true;
        assertTrue(false);
      }
    }
    rx = Serial.read();
    delay(10);
  }
  recu = true;
  assertTrue(true);
}

test(update_loop_success) {
  Serial.println("Start test update_loop_success. Enter 'y' to approve");
  char rx;
  int prevTorqueReq = 0;
  float variance = 1;
  while(rx != 'y') {
    classUnderTest.update();
    if (torqueRequest < prevTorqueReq - variance || torqueRequest > prevTorqueReq + variance) {
      Serial.print("Torque request changed to " + String(torqueRequest));
      prevTorqueReq = torqueRequest;
      if (motorMode == VCU1_MotorMode_POWERDRIVE){
        Serial.println("; Motor mode power drive");
        assertTrue((torqueRequest <= maxTorqueReq) && (torqueRequest >= minTorqueReq));
      } else {
        Serial.println("; Motor mode generate");
        assertTrue((torqueRequest <= maxNegTorqueReq) && (torqueRequest >= minTorqueReq));
      }
    }
    rx = Serial.read();
    delay(10);
  }
  assertTrue(true);
}

test(update_brakeFuncOn_success) {
  Serial.println("Start test update_brakeFuncOn_success. Enter 'y' to approve");
  EEPROM.put(BRAKE_PEDAL_FUNCTION, 1);
  classUnderTest.begin(throttlePin, brakePedalPin);
  char rx;
  int prevTorqueReq = 0;
  float variance = 1;
  while(rx != 'y') {
    classUnderTest.update();
    if (torqueRequest < prevTorqueReq - variance || torqueRequest > prevTorqueReq + variance) {
      Serial.print("Torque request changed to " + String(torqueRequest));
      prevTorqueReq = torqueRequest;
      if (motorMode == VCU1_MotorMode_POWERDRIVE){
        Serial.println("; Motor mode power drive");
        assertTrue((torqueRequest <= maxTorqueReq) && (torqueRequest >= minTorqueReq));
      } else {
        if (brakePedStatus == brakePedPressed) {
          Serial.print("; Brake pedal pressed");
        }
        Serial.println("; Motor mode generate");
        assertTrue((torqueRequest <= maxNegTorqueReq) && (torqueRequest >= minTorqueReq));
      }
    }
    rx = Serial.read();
    delay(10);
  }
  EEPROM.put(BRAKE_PEDAL_FUNCTION, 0);
  classUnderTest.begin(throttlePin);
  assertTrue(true);
}

test(update_brakeFuncOnRecuOff_success) {
  Serial.println("Start test update_brakeFuncOnRecuOff_success. Enter 'y' to approve");
  EEPROM.put(BRAKE_PEDAL_FUNCTION, 1);
  classUnderTest.begin(throttlePin, brakePedalPin);
  recu = false;
  char rx;
  int prevTorqueReq = 0;
  float variance = 1;
  while(rx != 'y') {
    classUnderTest.update();
    if (torqueRequest < prevTorqueReq - variance || torqueRequest > prevTorqueReq + variance) {
      Serial.print("Torque request changed to " + String(torqueRequest));
      prevTorqueReq = torqueRequest;
      if (brakePedStatus == brakePedPressed) {
        Serial.print("; Brake pedal pressed");
      }
      if (motorMode == VCU1_MotorMode_POWERDRIVE){
        Serial.println("; Motor mode power drive");
        assertTrue((torqueRequest <= maxTorqueReq) && (torqueRequest >= minTorqueReq));
      } else {
        Serial.println("; Motor mode generate");
        recu = true;
        assertTrue(false);
      }
    }
    rx = Serial.read();
    delay(10);
  }
  EEPROM.put(BRAKE_PEDAL_FUNCTION, 0);
  classUnderTest.begin(throttlePin);
  recu = true;
  assertTrue(true);
}

test(update_direcFalse_backward) {
  direc = false;
  classUnderTest.reset();
  classUnderTest.update();
  direc = true;
  int val = VCU1_GearLeverPosSts_R;
  assertEqual(directionRequest, val);
}

test(update_maxRpmExceeded_torqueReqZero) {
  motorRpm = 9999;
  classUnderTest.reset();
  classUnderTest.update();
  motorRpm = 2000;
  assertTrue(torqueRequest == 0 || (torqueRequest > 0 && motorMode == VCU1_MotorMode_GENERATE));
}

test(mock_readSignal) {
  assertEqual(mock.readSignal(1,1,1), motorRpm);
}

test(mock_writeSignal) {
  int val = 5;
  mock.writeSignal(VCU1, VCU1_GearLeverPosSts_LSB, VCU1_GearLeverPosSts_LEN, val);
  assertEqual(val, directionRequest);
  mock.writeSignal(VCU1, VCU1_MotorMode_LSB, VCU1_MotorMode_LEN, val);
  assertEqual(val, motorMode);
  mock.writeSignal(VCU1, VCU1_BrakePedalSts_LSB, VCU1_BrakePedalSts_LEN, val);
  assertEqual(val, brakePedStatus);
  mock.writeSignal(VCU1, VCU1_TorqueReq_LSB, VCU1_TorqueReq_LEN, val);
  assertEqual(val, torqueRequest);
}

test(mock_checkError) {
  assertEqual(mock.checkError(), true);
}
