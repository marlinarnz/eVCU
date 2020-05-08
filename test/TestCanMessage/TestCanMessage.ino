#line 2 "TestCanMessage.ino"

#include <AUnitVerbose.h>
#include <mcp_can.h>
#include "CanMessage.h"

using aunit::TestRunner;

MCP_CAN CAN(10);

uint32_t id = 0x7FF;
CanMessage classUnderTest(id, &CAN, 10);

float readSignalFail = -1;

// Prepare the serial port
void setup() {
    Serial.begin(9600);
    delay(100);
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

test(init_checkId_success) {
  assertEqual(classUnderTest.getId(),
              id);
}

// TODO: properly mock the sendMsgBuf function
/*test(send_noInterval_success) {
  uint8_t result = 1;
  assertEqual(classUnderTest.send(),
              result);
}

test(send_intervalHigh_success) {
  uint8_t result = 1;
  assertEqual(classUnderTest.send(1000),
              result);
}*/

test(send_extendedId) {
  skipTestNow();
}

test(send_frameTooLong) {
  skipTestNow();
}

test(readSignal_lsbTooHigh_fail) {
  assertEqual(classUnderTest.readSignal(65, 1),
              readSignalFail);
}

test(readSignal_lsbNegative_fail) {
  assertEqual(classUnderTest.readSignal(-1, 1),
              readSignalFail);
}

test(readSignal_lenLongerThanFrame_fail) {
  assertEqual(classUnderTest.readSignal(56, 65),
              readSignalFail);
}

test(readSignal_lenLongerThanLong_fail) {
  assertEqual(classUnderTest.readSignal(56, 33),
              readSignalFail);
}

test(readSignal_lenHigherThanLsb_fail) {
  assertEqual(classUnderTest.readSignal(0, 9),
              readSignalFail);
}

test(readSignal_lenHigherThanLsb6_fail) {
  assertEqual(classUnderTest.readSignal(6, 4),
              readSignalFail);
}

test(readSignal_lenZero_fail) {
  assertEqual(classUnderTest.readSignal(0, 0),
              readSignalFail);
}

test(readSignal_lenNegative_fail) {
  assertEqual(classUnderTest.readSignal(0, -1),
              readSignalFail);
}

test(readSignal_LsbLenCorrect_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(45, 23),
              zero);
}

test(readSignal_firstBit_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(0, 1),
              zero);
}

test(readSignal_firstByte_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(0, 8),
              zero);
}

test(readSignal_lastBit_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(63, 1),
              zero);
}

test(readSignal_lastByte_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(56, 8),
              zero);
}

test(readSignal_offsetPositive_success) {
  float result = -1000;
  assertEqual(classUnderTest.readSignal(25, 8, 1, 1000),
              result);
}

test(readSignal_offsetNegative_success) {
  float result = 1000;
  assertEqual(classUnderTest.readSignal(8, 8, 1, -1000),
              result);
}

test(readSignal_conv_returnZero) {
  float zero = 0;
  assertEqual(classUnderTest.readSignal(8, 8, 5),
              zero);
}

test(readSignal_testByte255_firstBit_success) {
  float result = 1;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.setFrameBytes(255); // 0b1111 1111
  assertEqual(classUnderTestLocal.readSignal(0, 1),
              result);
}

test(readSignal_testByte170_firstByte_success) {
  float result = 170;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTestLocal.readSignal(0, 8),
              result);
}

test(readSignal_testByte170_lastBit_success) {
  float result = 1;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTestLocal.readSignal(63, 1),
              result);
}

test(readSignal_testByte170_lastByte_success) {
  float result = 170;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTestLocal.readSignal(56, 8),
              result);
}

test(readSignal_testByte170_twoBytes_success) {
  float result = 43690;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTestLocal.readSignal(44, 16),
              result);
}

test(readSignal_testByte170_twoBytesUnevenLsb_success) {
  float result = 21845;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTestLocal.readSignal(45, 16),
              result);
}

test(readSignal_testByte170_twoBytesUnevenLsbLen12_success) {
  float result = 1365;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTestLocal.readSignal(45, 12),
              result);
}

test(readSignal_testByte170_twoBytesUnevenLsbLen13_success) {
  float result = 5461;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTestLocal.readSignal(45, 13),
              result);
}

test(readSignal_testByte170_fourBytes_success) {
  float result = 2863311530;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTestLocal.readSignal(58, 32),
              result);
}

test(readSignalWriteSignal_firstByte_success) {
  uint8_t val = 255 << 2;
  float result = float(val);
  int lsb = 0;
  int len = 8;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, val);
  assertEqual(classUnderTestLocal.readSignal(lsb, len),
              result);
}

test(readSignalWriteSignal_firstBit_success) {
  uint8_t val = 1;
  float result = float(val);
  int lsb = 0;
  int len = 1;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, val);
  assertEqual(classUnderTestLocal.readSignal(lsb, len),
              result);
}

test(readSignalWriteSignal_lastByte_success) {
  uint8_t val = 255 << 2;
  float result = float(val);
  int lsb = 56;
  int len = 8;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, val);
  assertEqual(classUnderTestLocal.readSignal(lsb, len),
              result);
}

test(readSignalWriteSignal_lastBit_success) {
  uint8_t val = 1;
  float result = float(val);
  int lsb = 63;
  int len = 1;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, val);
  assertEqual(classUnderTestLocal.readSignal(lsb, len),
              result);
}

test(readSignalWriteSignal_middleByte_success) {
  uint8_t val = 255 << 2;
  float result = float(val);
  int lsb = 43;
  int len = 8;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, val);
  assertEqual(classUnderTestLocal.readSignal(lsb, len),
              result);
}

test(readSignalWriteSignal_middle11Bits_success) {
  uint16_t val = 65530 >> 6;
  float result = float(val);
  int lsb = 27;
  int len = 11;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, val);
  assertEqual(classUnderTestLocal.readSignal(lsb, len),
              result);
}

test(readSignalWriteSignal_middle29Bits_success) {
  uint16_t val = 65530 >> 6;
  val <<= 10;
  float result = float(val);
  int lsb = 60;
  int len = 29;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, val);
  assertEqual(classUnderTestLocal.readSignal(lsb, len),
              result);
}

test(readSignalWriteSignal_convByte_success) {
  uint8_t val = 255;
  float conv = 0.392;
  float result = val * conv;
  int lsb = 16;
  int len = 8;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, val, conv);
  assertNear(classUnderTestLocal.readSignal(lsb, len),
             result, 0.1);
}

test(readSignalWriteSignal_offsetByte_success) {
  uint8_t val = 255;
  int offset = 1000;
  float result = val - offset;
  int lsb = 16;
  int len = 8;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, val, 1, offset);
  assertNear(classUnderTestLocal.readSignal(lsb, len),
             result, 0.1);
}

test(writeSignal_negativeConv_fail) {
  int lsb = 0;
  int len = 8;
  float result = 0;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, 255, -10);
  assertEqual(classUnderTestLocal.readSignal(lsb, len),
              result);
}

test(writeSignal_negativeOffset_fail) {
  int lsb = 0;
  int len = 8;
  float result = 0;
  CanMessage classUnderTestLocal(id, &CAN, 0);
  classUnderTestLocal.writeSignal(lsb, len, 255, 1, -100);
  assertEqual(classUnderTestLocal.readSignal(lsb, len),
              result);
}
