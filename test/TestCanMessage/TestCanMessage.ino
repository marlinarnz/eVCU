#line 2 "TestCanMessage.ino"

#include <AUnitVerbose.h>
#include <mcp_can.h>
#include "CanMessage.h"

using aunit::TestRunner;

MCP_CAN CAN(10);

uint32_t id = 0x7FF;
CanMessage classUnderTest(id, &CAN, 10);

float readSignalBEFail = -1;

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
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.getId(),
              id);
}

// TODO: properly mock the sendMsgBuf function
/*test(send_noInterval_success) {
  uint8_t result = 1;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.send(),
              result);
}

test(send_intervalHigh_success) {
  uint8_t result = 1;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.send(1000),
              result);
}*/

test(readByte_byteTooHigh_fail) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readByte(8),
              readSignalBEFail);
}

test(readByte_byteTooLow_fail) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readByte(-1),
              readSignalBEFail);
}

test(readByte_firstByte_success) {
  float result = 171;
  classUnderTest.setFrameBytes(171);
  assertEqual(classUnderTest.readByte(0),
              result);
}

test(readByte_lastByte_success) {
  float result = 112;
  classUnderTest.setFrameBytes(112);
  assertEqual(classUnderTest.readByte(7),
              result);
}

test(readByte_middleByte_success) {
  float result = 200;
  classUnderTest.setFrameBytes(200);
  assertEqual(classUnderTest.readByte(5),
              result);
}

test(writeByte_firstByte_success) {
  float result = 171;
  classUnderTest.writeByte(0, 171);
  assertEqual(classUnderTest.readByte(0),
              result);
}

test(writeByte_lastByte_success) {
  float result = 112;
  classUnderTest.writeByte(7, 112);
  assertEqual(classUnderTest.readByte(7),
              result);
}

test(writeByte_middleByte_success) {
  float result = 200;
  classUnderTest.writeByte(5, 200);
  assertEqual(classUnderTest.readByte(5),
              result);
}

test(readSignalBE_lsbTooHigh_fail) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(65, 1),
              readSignalBEFail);
}

test(readSignalBE_lsbNegative_fail) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(-1, 1),
              readSignalBEFail);
}

test(readSignalBE_lenLongerThanFrame_fail) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(56, 65),
              readSignalBEFail);
}

test(readSignalBE_lenLongerThanLong_fail) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(56, 33),
              readSignalBEFail);
}

test(readSignalBE_lenHigherThanLsb_fail) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(0, 9),
              readSignalBEFail);
}

test(readSignalBE_lenHigherThanLsb6_fail) {
  assertEqual(classUnderTest.readSignalBE(6, 4),
              readSignalBEFail);
}

test(readSignalBE_lenZero_fail) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(0, 0),
              readSignalBEFail);
}

test(readSignalBE_lenNegative_fail) {
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(0, -1),
              readSignalBEFail);
}

test(readSignalBE_LsbLenCorrect_returnZero) {
  float zero = 0;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(45, 23),
              zero);
}

test(readSignalBE_firstBit_returnZero) {
  float zero = 0;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(0, 1),
              zero);
}

test(readSignalBE_firstByte_returnZero) {
  float zero = 0;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(0, 8),
              zero);
}

test(readSignalBE_lastBit_returnZero) {
  float zero = 0;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(63, 1),
              zero);
}

test(readSignalBE_lastByte_returnZero) {
  float zero = 0;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(56, 8),
              zero);
}

test(readSignalBE_offsetPositive_success) {
  float result = -1000;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(25, 8, 1, 1000),
              result);
}

test(readSignalBE_offsetNegative_success) {
  float result = 1000;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(8, 8, 1, -1000),
              result);
}

test(readSignalBE_conv_returnZero) {
  float zero = 0;
  classUnderTest.setFrameBytes(0);
  assertEqual(classUnderTest.readSignalBE(8, 8, 5),
              zero);
}

test(setFrameBytes_zero_success) {
  float zero = 0;
  classUnderTest.setFrameBytes(0);
  for (int i=0; i<8; i++) {
    assertEqual(classUnderTest.readSignalBE(i * 8, 8),
                zero);
  }
}

test(setFrameBytes_255_success) {
  float result = 255;
  classUnderTest.setFrameBytes(255);
  for (int i=0; i<8; i++) {
    assertEqual(classUnderTest.readSignalBE(i * 8, 8),
                result);
  }
}

test(setFrameBytes_171_success) {
  float result = 171;
  classUnderTest.setFrameBytes(171);
  for (int i=0; i<8; i++) {
    assertEqual(classUnderTest.readSignalBE(i * 8, 8),
                result);
  }
}

test(readSignalBE_testByte255_firstBit_success) {
  float result = 1;
  classUnderTest.setFrameBytes(255); // 0b1111 1111
  assertEqual(classUnderTest.readSignalBE(0, 1),
              result);
}

test(readSignalBE_testByte170_firstByte_success) {
  float result = 170;
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(0, 8),
              result);
}

test(readSignalBE_testByte170_lastBit_success) {
  float result = 1;
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(63, 1),
              result);
}

test(readSignalBE_testByte170_lastByte_success) {
  float result = 170;
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(56, 8),
              result);
}

test(readSignalBE_testByte170_twoBytes_success) {
  float result = 43690;
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(44, 16),
              result);
}

test(readSignalBE_testByte170_twoBytesUnevenLsb_success) {
  float result = 21845; // 0b0 1010 1010 1010 101
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(45, 16),
              result);
}

test(readSignalBE_testByte170_unevenLsbLen12_success) {
  float result = 1365; // 0b0 1010 1010 101
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(45, 12),
              result);
}

test(readSignalBE_testByte170_twoBytesUnevenLsbLen13_success) {
  float result = 5461; // 0b10 1010 1010 101
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(45, 13),
              result);
}

test(readSignalBE_testByte170_threeBytes_success) {
  float result = 11184810;
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(34, 24),
              result);
}

test(readSignalBE_testByte170_threeBytesOdd_success) {
  float result = 5592405;
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(35, 24),
              result);
}

test(readSignalBE_testByte170_fourBytes_success) {
  float result = 2863311530;
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(56, 32),
              result);
}

test(readSignalBE_testByte170_fourBytesOdd_success) {
  float result = 2863311530;
  classUnderTest.setFrameBytes(170); // 0b1010 1010
  assertEqual(classUnderTest.readSignalBE(59, 32),
              result);
}

test(readSignalBEWriteSignal_firstByte_success) {
  uint8_t val = 255 << 2;
  float result = float(val);
  int lsb = 0;
  int len = 8;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(lsb, len),
              result);
}

test(readSignalBEWriteSignal_firstBit_success) {
  uint8_t val = 1;
  float result = float(val);
  int lsb = 0;
  int len = 1;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(lsb, len),
              result);
}

test(readSignalBEWriteSignal_firstBitPeriphery_success) {
  uint8_t val = 1;
  float left = 85; // 0b1010 101
  int lsb = 0;
  int len = 1;
  classUnderTest.setFrameBytes(170);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(1, 7),
              left);
}

test(readSignalBEWriteSignal_lastByte_success) {
  uint8_t val = 255 << 2;
  float result = float(val);
  int lsb = 56;
  int len = 8;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(lsb, len),
              result);
}

test(readSignalBEWriteSignal_lastBit_success) {
  uint8_t val = 1;
  float result = float(val);
  int lsb = 63;
  int len = 1;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(lsb, len),
              result);
}

test(readSignalBEWriteSignal_lastBitPeriphery_success) {
  uint8_t val = 1;
  float right = 42; // 0b010 1010
  int lsb = 63;
  int len = 1;
  classUnderTest.setFrameBytes(170);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(56, 7),
              right);
}

test(readSignalBEWriteSignal_middleByte_success) {
  uint8_t val = 255 << 2;
  float result = float(val);
  int lsb = 43;
  int len = 8;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(lsb, len),
              result);
}

test(readSignalBEWriteSignal_middle11Bits_success) {
  uint16_t val = 65530 >> 6;
  float result = float(val);
  int lsb = 27;
  int len = 11;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(lsb, len),
              result);
}

test(readSignalBEWriteSignal_middle29Bits_success) {
  uint16_t val = 65530 >> 6;
  val <<= 10;
  float result = float(val);
  int lsb = 60;
  int len = 29;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(lsb, len),
              result);
}

test(readSignalBEWriteSignal_middle11BitsPeriphery_success) {
  uint16_t val = 65530;
  float right = 2; // 0b010
  float left = 2; // 0b10
  int lsb = 27;
  int len = 11;
  classUnderTest.setFrameBytes(170);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(24, 3),
              right);
  assertEqual(classUnderTest.readSignalBE(22, 2),
              left);
}

test(readSignalBEWriteSignal_middle29BitsPeriphery_success) {
  uint16_t val = 65530;
  float right = 10; // 0b1010
  float left = 85; // 0b1010 101
  int lsb = 60;
  int len = 29;
  classUnderTest.setFrameBytes(170);
  classUnderTest.writeSignal(lsb, len, val);
  assertEqual(classUnderTest.readSignalBE(56, 4),
              right);
  assertEqual(classUnderTest.readSignalBE(33, 7),
              left);
}

test(readSignalBEWriteSignal_convByte_success) {
  uint8_t val = 90;
  float conv = 0.392;
  float result = val / conv;
  int lsb = 16;
  int len = 8;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val, conv);
  assertNear(classUnderTest.readSignalBE(lsb, len),
             result, 1.0);
}

test(readSignalBEWriteSignal_convByteImplicit_success) {
  uint8_t val = 90;
  float conv = 0.392;
  float result = val;
  int lsb = 16;
  int len = 8;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val, conv);
  assertNear(classUnderTest.readSignalBE(lsb, len, conv),
             result, 0.5);
}

test(readSignalBEWriteSignal_offsetByte_success) {
  int val = -10;
  int offset = 40;
  float result = val + offset;
  int lsb = 16;
  int len = 8;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val, 1, offset);
  assertNear(classUnderTest.readSignalBE(lsb, len),
             result, 0.1);
}

test(readSignalBEWriteSignal_offsetByteImplicit_success) {
  int val = -10;
  int offset = 40;
  float result = val;
  int lsb = 16;
  int len = 8;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, val, 1, offset);
  assertNear(classUnderTest.readSignalBE(lsb, len, 1, offset),
             result, 0.1);
}

test(writeSignal_negativeConv_fail) {
  int lsb = 0;
  int len = 8;
  float result = 0;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, 255, -10);
  assertEqual(classUnderTest.readSignalBE(lsb, len),
              result);
}

test(writeSignal_negativeOffset_fail) {
  int lsb = 0;
  int len = 8;
  float result = 0;
  classUnderTest.setFrameBytes(0);
  classUnderTest.writeSignal(lsb, len, 255, 1, -100);
  assertEqual(classUnderTest.readSignalBE(lsb, len),
              result);
}
