#line 2 "TestAccessControl.ino"

#include <AUnitVerbose.h>
#include "AccessControl.h"
#include "AccessLock.h"

using aunit::TestRunner;

AccessControl semphrNum;
int num = 1;
int five = 5;
int eight = 8;

// A function locks a variable and changes it
void changeNumTo5( void *pvParameters ) {
  (void) pvParameters;
  for (;;) {
  semphrNum.lock();
  num = five;
  vTaskDelay(10);
  semphrNum.unlock();
  }
}
void changeNumTo8( void *pvParameters ) {
  (void) pvParameters;
  for (;;) {
  semphrNum.lock();
  num = eight;
  semphrNum.unlock();
  vTaskDelay(3);
  }
}


// Prepare the serial port, include/exclude tests
void setup() {
    Serial.begin(9600);
    delay(50);
    TestRunner::setTimeout(120);
    //TestRunner::exclude("*");
    //TestRunner::include("*");
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}

test(AccessControlTest, test_constructor) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    AccessControl semphr1;
    AccessControl semphr2();
  }
  assertTrue(true);
}

test(AccessControlTest, test_lock) {
  if(true) {
    AccessControl semphr1;
    semphr1.lock();
  }
  assertTrue(true);
}

test(AccessControlTest, test_unlock_locked) {
  if(true) {
    AccessControl semphr1;
    semphr1.lock();
    semphr1.unlock();
  }
  assertTrue(true);
}

test(AccessControlTest, test_unlock_unlocked) {
  if(true) {
    AccessControl semphr1;
    semphr1.unlock();
  }
  assertTrue(true);
}


test(AccessControlTest, test_lock_multitaskingPrio0) {
  // Test this for different thread priorities
  // Create two threads that want to change global variable num
  TaskHandle_t Task1, Task2;
  xTaskCreate(changeNumTo5, "changeTo5", 1024, NULL, 0, &Task1);
  xTaskCreate(changeNumTo8, "changeTo8", 1024, NULL, 0, &Task2);
  delay(50);
  vTaskDelete(Task1);
  vTaskDelete(Task2);
  assertEqual(num, 5);
}


test(AccessControlTest, test_lock_multitaskingPrio1) {
  // Test this for different thread priorities
  // Create two threads that want to change global variable num
  TaskHandle_t Task1, Task2;
  xTaskCreate(changeNumTo5, "changeTo5", 1024, NULL, 0, &Task1);
  xTaskCreate(changeNumTo8, "changeTo8", 1024, NULL, 1, &Task2);
  delay(50);
  vTaskDelete(Task1);
  vTaskDelete(Task2);
  assertEqual(num, 5);
}


test(AccessControlTest, test_lock_multitaskingPrio2) {
  // Test this for different thread priorities
  // Create two threads that want to change global variable num
  TaskHandle_t Task1, Task2;
  xTaskCreate(changeNumTo5, "changeTo5", 1024, NULL, 0, &Task1);
  xTaskCreate(changeNumTo8, "changeTo8", 1024, NULL, 2, &Task2);
  delay(50);
  vTaskDelete(Task1);
  vTaskDelete(Task2);
  assertEqual(num, 5);
}

test(AccessLockTest, test_constructor) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    AccessControl semphr1;
    AccessLock lock(&semphr1);
  }
  assertTrue(true);
}
