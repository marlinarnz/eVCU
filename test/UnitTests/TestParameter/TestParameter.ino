#line 2 "TestParameter.ino"

#include <AUnitVerbose.h>
#include "AccessControl.h"
#include "Parameter.h"

using aunit::TestRunner;


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


test(ParameterTest, test_constructor) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    Parameter p(0);
    Parameter* pp = new Parameter(0);
    delete pp;
  }
  assertTrue(true);
}


test(ParameterBoolTest, test_constructor) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    ParameterBool p(5);
    ParameterBool* pp = new ParameterBool(0);
    delete pp;
  }
  assertTrue(true);
}


test(ParameterIntTest, test_constructor) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    ParameterInt p(5);
    ParameterInt* pp = new ParameterInt(0);
    delete pp;
  }
  assertTrue(true);
}


test(ParameterDoubleTest, test_constructor) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    ParameterDouble p(5);
    ParameterDouble* pp = new ParameterDouble(0);
    delete pp;
  }
  assertTrue(true);
}


test(ParameterTest, test_getId) {
  int id = 5;
  Parameter p(id);
  assertEqual(id, p.getId());
}


test(ParameterBoolTest, test_getId) {
  int id = 5;
  ParameterBool p(id);
  assertEqual(id, p.getId());
}


test(ParameterIntTest, test_getId) {
  int id = 5;
  ParameterInt p(id);
  assertEqual(id, p.getId());
}


test(ParameterDoubleTest, test_getId) {
  int id = 5;
  ParameterDouble p(id);
  assertEqual(id, p.getId());
}


test(ParameterBoolTest, test_getVal) {
  bool val = false;
  ParameterBool p(0);
  assertEqual(val, p.getVal());
}


test(ParameterIntTest, test_getVal) {
  int val = 0;
  ParameterInt p(0);
  assertEqual(val, p.getVal());
}


test(ParameterDoubleTest, test_getVal) {
  double val = 0;
  ParameterDouble p(0);
  assertEqual(val, p.getVal());
}


test(ParameterBoolTest, test_setVal) {
  bool val = 1;
  ParameterBool p(0);
  p.setVal(val);
  assertTrue(true);
}


test(ParameterIntTest, test_setVal) {
  int val = 1000;
  ParameterInt p(0);
  p.setVal(val);
  assertTrue(true);
}


test(ParameterDoubleTest, test_setVal) {
  double val = 0.1;
  ParameterDouble p(0);
  p.setVal(val);
  assertTrue(true);
}


test(ParameterBoolTest, test_setAndGetVal) {
  bool val = 1;
  ParameterBool p(0);
  p.setVal(val);
  assertEqual(val, p.getVal());
}


test(ParameterIntTest, test_setAndGetVal) {
  int val = 1000;
  ParameterInt p(0);
  p.setVal(val);
  assertEqual(val, p.getVal());
}


test(ParameterDoubleTest, test_setAndGetVal) {
  double val = 0.1;
  ParameterDouble p(0);
  p.setVal(val);
  assertEqual(val, p.getVal());
}
