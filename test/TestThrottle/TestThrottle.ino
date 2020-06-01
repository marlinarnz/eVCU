#line 2 "TestThrottle.ino"

#include <AUnitVerbose.h>
#include "constants.h"
#include "CanManager.h"
#include "commUtils.h"
#include "Throttle.h"

using aunit::TestRunner;

bool recu = true;
bool direc = true;

// RPM that the throttle reads
float motorRpm = 2000;

// Values that the throttle writes
int motorMode;
int brakePedStatus;
int torqueRequest;
int directionRequest;

CanManager mock;
Throttle classUnderTest(&mock);

// Prepare the serial port
void setup() {
    Serial.begin(9600);
    TestRunner::setTimeout(180);
    resetEEPROM();
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
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
