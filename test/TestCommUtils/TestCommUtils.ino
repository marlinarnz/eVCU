#line 2 "TestCommUtils.ino"

#include <AUnitVerbose.h>
#include <EEPROM.h>
#include "commUtils.h"
#include "constants.h"

using aunit::TestRunner;

// Prepare the serial port
void setup() {
    Serial.begin(9600);
    delay(50);
    TestRunner::setTimeout(60);
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

test(setWarningLevel_0_success) {
  int lvl = 0;
  setWarningLevel(lvl);
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL),
              lvl);
}

test(setWarningLevel_2_success) {
  int lvl = 2;
  setWarningLevel(lvl);
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL),
              lvl);
}

test(setWarningLevel_3_success) {
  int lvl = 3;
  setWarningLevel(lvl);
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL),
              lvl);
}

test(setWarningLevel_1_success) {
  int lvl = 1;
  setWarningLevel(lvl);
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL),
              lvl);
}

test(setWarningLevel_minus1_nothingHappens) {
  int lvl = -1;
  int result = EEPROM.read(VCU1_WARNING_LEVEL);
  setWarningLevel(lvl);
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL),
              result);
}

test(setWarningLevel_4_nothingHappens) {
  int lvl = 4;
  int result = EEPROM.read(VCU1_WARNING_LEVEL);
  setWarningLevel(lvl);
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL),
              result);
}

test(resetEEPROM_all_success) {
  for(int i=0; i<=BRAKE_PEDAL_FUNCTION; i++) {
    EEPROM.write(i, 111);
  }
  resetEEPROM();
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL), vcu1WarningLevel);
  assertEqual(EEPROM.read(COOL_THRESHOLD), coolThreshold);
  assertEqual(EEPROM.read(MAX_TORQUE), maxTorque);
  assertEqual(EEPROM.read(MAX_NEG_TORQUE), maxNegTorque);
  assertEqual(EEPROM.read(MAX_RPM), maxRPM);
  assertEqual(EEPROM.read(BRAKE_LIGHT_TORQUE_THRESHOLD), brakeLightTorqueThreshold);
  assertEqual(EEPROM.read(THROTTLE_POS_MAX_TORQUE), throttlePosMaxTorque);
  assertEqual(EEPROM.read(THROTTLE_POS_HALF_TORQUE), throttlePosHalfTorque);
  assertEqual(EEPROM.read(THROTTLE_POS_ZERO_TORQUE), throttlePosZeroTorque);
  assertEqual(EEPROM.read(THROTTLE_POS_ZERO_NEG_TORQUE), throttlePosZeroNegTorque);
  assertEqual(EEPROM.read(THROTTLE_POS_MAX_NEG_TORQUE), throttlePosMaxNegTorque);
  assertEqual(EEPROM.read(THROTTLE_POS_START), throttlePosStart);
  assertEqual(EEPROM.read(BRAKE_PEDAL_FUNCTION), brakePedalFunction);
}

test(report_levelTooHigh_fail) {
  report("This message should not appear but fail", 4);
  Serial.println("Have you seen a message on the serial? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'n';
  assertEqual(rx, result);
}

test(report_levelNegative_fail) {
  report("This message should not appear but fail", -1);
  Serial.println("Have you seen a message on the serial? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'n';
  assertEqual(rx, result);
}

test(report_level0_success) {
  report("This message should appear", 0);
  Serial.println("Have you seen a message on the serial? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'y';
  assertEqual(rx, result);
}

test(report_level1_success) {
  report("This message should appear", 1);
  Serial.println("Have you seen a message on the serial? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'y';
  assertEqual(rx, result);
}

test(report_level2_success) {
  report("This message should appear", 2);
  Serial.println("Have you seen a message on the serial? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'y';
  assertEqual(rx, result);
}

test(report_level3_success) {
  report("This message should appear", 3);
  Serial.println("Have you seen a message on the serial? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'y';
  assertEqual(rx, result);
}

test(report_warnLevel3_success) {
  report("This message should appear", 3, true);
  int result = 3;
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL), result);
}

test(report_warnLevel0_success) {
  report("This message should appear", 0, true);
  int result = 0;
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL), result);
}

test(report_warnLevel4_nothingHappens) {
  int result = EEPROM.read(VCU1_WARNING_LEVEL);
  report("This message should appear", 4, true);
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL), result);
}

test(report_warnLevelMinus1_nothingHappens) {
  int result = EEPROM.read(VCU1_WARNING_LEVEL);
  report("This message should appear", -1, true);
  assertEqual(EEPROM.read(VCU1_WARNING_LEVEL), result);
}
