#line 2 "TestCanMessage.ino"

#include <AUnitVerbose.h>
#include <mcp_can.h>
#include "CanManager.h"

using aunit::TestRunner;

MCP_CAN CAN(10);

float readSignalFail = -1;

// Prepare the serial port
void setup() {
    Serial.begin(9600);
    delay(50);
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

/*test(init_checkId_success) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.getId(),
              id);
}*/
