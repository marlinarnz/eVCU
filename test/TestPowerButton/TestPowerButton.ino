#line 2 "TestPowerButton.ino"

#include <AUnitVerbose.h>
#include "constants.h"
#include "CanManager.h"
#include "PowerButton.h"

using aunit::TestRunner;

bool on = false;
unsigned long prechargeStart = 0;
bool returnCheckError = CAN_OK;
float returnReadSignal = 0;

CanManager mock;
PowerButton classUnderTest(&mock);

// Prepare the serial port
void setup() {
    Serial.begin(9600);
    delay(50);
    TestRunner::setTimeout(180);
    Serial.println("In case nothing happens on the Serial, there is a loop test at work that can be ended by entering [y].");
    classUnderTest.begin(powerButtonPin,powerButtonLEDPin,true);
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

test(startMotor_MCUnotReady_fail) {
  classUnderTest.setMCUready(false);
  classUnderTest.startMotor();
  assertTrue(!on);
}

test(startMotor_MCUReady_success) {
  classUnderTest.setMCUready(true);
  classUnderTest.startMotor();
  assertTrue(on);
}

test(stopMotor_MCUReady_success) {
  classUnderTest.setMCUready(true);
  classUnderTest.startMotor();
  assertTrue(on);
  classUnderTest.stopMotor();
  assertTrue(!on);
}

testing(update_MCUReady_onOffLightsWork) {
  classUnderTest.setMCUready(true);
  classUnderTest.update();
  char rx;
  rx = Serial.read();
  if (rx == 'y') {
    pass();
  }
}

testing(update_MCUnotReady_errorLight) {
  classUnderTest.setMCUready(false);
  classUnderTest.update();
  char rx;
  rx = Serial.read();
  if (rx == 'y') {
    pass();
  }
}

/*test(userinteraction) {
  Serial.println("Have you seen a message on the serial? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'y';
  assertEqual(rx, result);
}*/
