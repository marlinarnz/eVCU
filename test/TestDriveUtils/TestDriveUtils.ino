#line 2 "TestDriveUtils.ino"

#include <AUnitVerbose.h>
#include <EEPROM.h>
#include "driveUtils.h"
#include "constants.h"

using aunit::TestRunner;

bool recu;
bool direc;

// Prepare the serial port
void setup() {
    Serial.begin(9600);
    delay(50);
    TestRunner::setTimeout(120);

    pinMode(coolantCyclePin, OUTPUT);
    pinMode(direcPin, INPUT);
    pinMode(recuPin, INPUT);

    EEPROM.write(COOL_THRESHOLD, coolThreshold);
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

test(cool_bothLow_pumpOff) {
  cool(30, 30);
  Serial.println("Is the internal LED alight? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'n';
  assertEqual(rx, result);
}

test(cool_bothHigh_pumpOn) {
  cool(50, 60);
  Serial.println("Is the internal LED alight? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'y';
  assertEqual(rx, result);
}

test(cool_T1HighT2Low_pumpOn) {
  cool(45, 30);
  Serial.println("Is the internal LED alight? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'y';
  assertEqual(rx, result);
}

test(cool_T1LowT2High_pumpOn) {
  cool(30, 77);
  Serial.println("Is the internal LED alight? [y/n]");
  char rx;
  while(rx != 'y' && rx != 'n') {
    delay(10);
    rx = Serial.read();
  }
  char result = 'y';
  assertEqual(rx, result);
}

test(updateRecu_switchHigh_recuTrue) {
  recu = false;
  Serial.println("Put the recuperation switch (pin 23) to HIGH (recu on)");
  while(digitalRead(recuPin) == LOW) {
    delay(10);
  }
  updateRecu();
  assertTrue(recu);
}

test(updateRecu_switchLow_recuFalse) {
  recu = true;
  Serial.println("Put the recuperation switch (pin 23) to LOW (recu off)");
  while(digitalRead(recuPin) == HIGH) {
    delay(10);
  }
  updateRecu();
  assertTrue(!recu);
}

test(updateDirec_switchHigh_direcFalse) {
  direc = true;
  Serial.println("Put the direction switch (pin 22) to HIGH (direc backward)");
  while(digitalRead(direcPin) == LOW) {
    delay(10);
  }
  updateDirec();
  assertTrue(!direc);
}

test(updateDirec_switchLow_direcTrue) {
  direc = false;
  Serial.println("Put the direction switch (pin 22) to LOW (direc forward)");
  while(digitalRead(direcPin) == HIGH) {
    delay(10);
  }
  updateDirec();
  assertTrue(direc);
}
