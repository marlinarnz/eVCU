#line 2 "TestCanManager.ino"

#include <AUnitVerbose.h>
#include <mcp2515.h>
#include "CanManager.h"
#include "commUtils.h"
#include "constants.h"

using aunit::TestRunner;

// Prepare the serial port
void setup() {
    Serial.begin(9600);
    delay(50);
    //report("test",0);
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

/*
 * These functions cannot be unit tested as they require a CAN:
 * begin(), sendMessage(), update(), checkError()
*/

MCP2515 CAN(10);

float readSignalFail = -1;

CanManager classUnderTest(&CAN);

test(init__success) {
  uint32_t id;
  id = VCU1;
  assertEqual(classUnderTest.messagesVCU[0].getId(), id);
  id = VCU2;
  assertEqual(classUnderTest.messagesVCU[1].getId(), id);
  id = VCU3;
  assertEqual(classUnderTest.messagesVCU[2].getId(), id);
  id = MCU1;
  assertEqual(classUnderTest.messagesOther[0].getId(), id);
  id = MCU2;
  assertEqual(classUnderTest.messagesOther[1].getId(), id);
  id = MCU3;
  assertEqual(classUnderTest.messagesOther[2].getId(), id);
  id = UI_ThrottlePos;
  assertEqual(classUnderTest.messagesOther[3].getId(), id);
  id = UI_DriveSettings;
  assertEqual(classUnderTest.messagesOther[4].getId(), id);
}

test(checkMCUready_normal_fail) {
  classUnderTest.begin();
  assertFalse(classUnderTest.checkMCUready());
}

test(checkMCUready_quick_fail) {
  classUnderTest.begin();
  assertFalse(classUnderTest.checkMCUready(true));
}

test(checkBMSready_normal_fail) {
  classUnderTest.begin();
  assertFalse(classUnderTest.checkBMSready());
}

test(checkBMSready_quick_fail) {
  classUnderTest.begin();
  assertFalse(classUnderTest.checkBMSready(true));
}

test(checkMCUwarning__fail) {
  classUnderTest.begin();
  assertFalse(classUnderTest.checkMCUwarning());
}

test(readSignal_lsbTooHigh_fail) {
  assertEqual(classUnderTest.readSignal(MCU1, 65, 1),
              readSignalFail);
}

test(readSignal_lsbNegative_fail) {
  assertEqual(classUnderTest.readSignal(MCU1, -1, 1),
              readSignalFail);
}

test(readSignal_lenLongerThanFrame_fail) {
  assertEqual(classUnderTest.readSignal(MCU2, 56, 65),
              readSignalFail);
}

test(readSignal_lenLongerThanLong_fail) {
  assertEqual(classUnderTest.readSignal(MCU1, 56, 33),
              readSignalFail);
}

test(readSignal_lenHigherThanLsb_fail) {
  assertEqual(classUnderTest.readSignal(MCU1, 0, 9),
              readSignalFail);
}

test(readSignal_lenHigherThanLsb6_fail) {
  assertEqual(classUnderTest.readSignal(MCU1, 6, 4),
              readSignalFail);
}

test(readSignal_lenZero_fail) {
  assertEqual(classUnderTest.readSignal(MCU1, 0, 0),
              readSignalFail);
}

test(readSignal_lenNegative_fail) {
  assertEqual(classUnderTest.readSignal(MCU1, 0, -1),
              readSignalFail);
}

test(readSignal_wrongId_fail) {
  uint32_t id = 0x00F;
  assertEqual(classUnderTest.readSignal(id, 0, 8),
              readSignalFail);
}

test(readSignal_MCU1_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(MCU1, 45, 23),
              zero);
}

test(readSignal_MCU2_DrvMotorOverCoolFault_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(MCU2, MCU2_DrvMotorOverCoolFault_LSB, MCU2_DrvMotorOverCoolFault_LEN),
              zero);
}

test(readSignal_MCU3_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(MCU3, 54, 13),
              zero);
}

test(readSignal_UI_ThrottlePos_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(UI_ThrottlePos, 8, 3),
              zero);
}

test(readSignal_UI_DriveSettings_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(UI_DriveSettings, 16, 17),
              zero);
}

test(readSignal_MCU1_ActMotorTq_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(MCU1, MCU1_ActMotorTq_LSB, MCU1_ActMotorTq_LEN),
              zero);
}

test(readSignal_MCU1_ActMotorSpd_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(MCU1, MCU1_ActMotorSpd_LSB, MCU1_ActMotorSpd_LEN, MCU1_ActMotorSpd_CONV_D),
              zero);
}

test(readSignal_MCU2_MotorTemp_returnOffset) {
  float offset = -40;
  assertEqual(classUnderTest.readSignal(MCU2, MCU2_MotorTemp_LSB, MCU2_MotorTemp_LEN, 1, MCU2_MotorTemp_OFFSET),
              offset);
}

test(readSignal_MCU2_WarningLevel_success) {
  float result = 3;
  classUnderTest.messagesOther[1].writeByte(int(MCU2_WarningLevel_LSB/8), 255);
  assertEqual(classUnderTest.readSignal(MCU2, MCU2_WarningLevel_LSB, MCU2_WarningLevel_LEN),
              result);
  classUnderTest.messagesOther[1].setFrameBytes(0);
}

test(readSignal_UI_DriveSettings_success) {
  float result = 43690;
  classUnderTest.messagesOther[4].setFrameBytes(170);
  assertEqual(classUnderTest.readSignal(UI_DriveSettings, 18, 17),
              result);
  classUnderTest.messagesOther[4].setFrameBytes(0);
}

test(readSignal_MCU1_ActMotorTq_success) {
  float result = 85;
  classUnderTest.messagesOther[0].writeByte(MCU1_ActMotorTq_LSB/8, int(result), MCU1_ActMotorTq_CONV_D);
  assertNear(classUnderTest.readSignal(MCU1, MCU1_ActMotorTq_LSB, MCU1_ActMotorTq_LEN, MCU1_ActMotorTq_CONV_D),
             result, 0.8);
  classUnderTest.messagesOther[0].setFrameBytes(0);
}

test(readSignal_MCU1_ActMotorSpd_success) {
  float result = 10922.5;
  classUnderTest.messagesOther[0].setFrameBytes(170);
  assertEqual(classUnderTest.readSignal(MCU1, MCU1_ActMotorSpd_LSB, MCU1_ActMotorSpd_LEN, MCU1_ActMotorSpd_CONV_D),
              result);
  classUnderTest.messagesOther[0].setFrameBytes(0);
}

test(readSignal_MCU2_MotorTemp_success) {
  float result = 35;
  classUnderTest.messagesOther[1].writeByte(MCU2_MotorTemp_LSB/8, int(result), 1, MCU2_MotorTemp_OFFSET);
  assertEqual(classUnderTest.readSignal(MCU2, MCU2_MotorTemp_LSB, MCU2_MotorTemp_LEN, 1, MCU2_MotorTemp_OFFSET),
              result);
  classUnderTest.messagesOther[1].setFrameBytes(0);
}

test(writeSignal_allMessagesOneByte_success) {
  for (uint8_t i=0; i<N_VCU_MESSAGES; i++) {
    for (uint8_t lsb=8; lsb<8*8; lsb++) {
      classUnderTest.writeSignal(classUnderTest.messagesVCU[i].getId(), lsb, 8, 170);
      float result = 170;
      assertEqual(classUnderTest.messagesVCU[i].readSignalBE(lsb, 8),
                  result);
    }
    classUnderTest.messagesVCU[i].setFrameBytes(0);
  }
}

test(writeSignal_conv_success) {
  float result = 777;
  float conv = 0.7;
  classUnderTest.writeSignal(VCU1, 47, 17, int(result), conv);
  assertNear(classUnderTest.messagesVCU[0].readSignalBE(47, 17, conv),
             result, 0.77);
  classUnderTest.messagesVCU[0].setFrameBytes(0);
}

test(writeSignal_offset_success) {
  float result = 77;
  float conv = 1;
  int offset = 7;
  classUnderTest.writeSignal(VCU1, 12, 7, int(result), conv, offset);
  assertNear(classUnderTest.messagesVCU[0].readSignalBE(12, 7, conv, offset),
             result, 0.77);
  classUnderTest.messagesVCU[0].setFrameBytes(0);
}
