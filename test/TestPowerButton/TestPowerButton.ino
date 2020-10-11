#line 2 "TestPowerButton.ino"

#include <AUnitVerbose.h>
#include "constants.h"
#include "CanManager.h"
#include "PowerButton.h"

using aunit::TestRunner;

bool on = false;
unsigned long prechargeStart = 0;
bool returnCheckError = CAN_OK;

CanManager mock;
PowerButton classUnderTest(&mock);

void updateButton(int del) {
  char rx;
  while(rx != 'y') {
    classUnderTest.update();
    rx = Serial.read();
    delay(del);
  }
}

// Prepare the serial port
void setup() {
    Serial.begin(9600);
    TestRunner::setTimeout(240);
    classUnderTest.begin(powerButtonPin,powerButtonLEDPin);
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

test(startMotor_MCUnotReady_fail) {
  on = false;
  returnCheckError = 1;
  classUnderTest.startMotor();
  assertTrue(!on);
}

test(startMotor_MCUReady_success) {
  on = false;
  returnCheckError = CAN_OK;
  classUnderTest.startMotor();
  assertTrue(on);
}

test(stopMotor_MCUReady_success) {
  on = true;
  returnCheckError = CAN_OK;
  classUnderTest.stopMotor();
  assertTrue(!on);
}

test(stopMotor_MCUNotReady_success) {
  on = true;
  returnCheckError = 1;
  classUnderTest.stopMotor();
  assertTrue(!on);
}

test(update_MCUReady_onOffWorks) {
  Serial.println("Running test update_MCUReady_onOffWorks until 'y' is entered.");
  on = false;
  returnCheckError = CAN_OK;
  updateButton(3);
  assertTrue(true);
}

test(update_MCUReadyLongDelay_onOffWorks) {
  Serial.println("Running test update_MCUReadyLongDelay_onOffWorks until 'y' is entered.");
  on = false;
  returnCheckError = CAN_OK;
  updateButton(14);
  assertTrue(true);
}

test(update_MCUnotReady_errorLight) {
  Serial.println("Running test update_MCUnotReady_errorLight until 'y' is entered.");
  on = false;
  returnCheckError = 1;
  classUnderTest.stopMotor();
  updateButton(4);
  assertTrue(true);
}

test(mock_readSignal) {
  float zero = 0;
  float one = OBC1_StartStatus_ON;
  assertEqual(mock.readSignal(1,1,1), zero);
  assertEqual(mock.readSignal(OBC1, OBC1_StartStatus_LSB, OBC1_StartStatus_LSB), one);
}

test(mock_writeSignal) {
  on = false;
  mock.writeSignal(VCU1, VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN, VCU1_KeyPosition_CRANK);
  assertTrue(on);
  mock.writeSignal(VCU1, VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN, VCU1_KeyPosition_ACC);
  assertTrue(!on);
}

test(mock_checkError) {
  assertEqual(mock.checkError(), returnCheckError==CAN_OK);
}
