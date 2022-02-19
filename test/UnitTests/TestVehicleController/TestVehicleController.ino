#line 2 "TestVehicleController.ino"

#include <AUnitVerbose.h>
#include "Parameter.h"
#include "VehicleController.h"
#include "Device.h"

using aunit::TestRunner;


Parameter param(0);
ParameterBool paramBool(1);
ParameterInt paramInt(2);
ParameterDouble paramDouble(3);


// Prepare the serial port, include/exclude tests
void setup() {
    Serial.begin(9600);
    delay(100);
    TestRunner::setTimeout(120);
    //TestRunner::exclude("*");
    //TestRunner::include("*");
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}


test(VehicleControllerTest, test_constructor_stack) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    VehicleController vc = VehicleController();
  }
  assertTrue(true);
}


test(VehicleControllerTest, test_constructor_heap) {
  VehicleController* vc = new VehicleController();
  delete vc;
  assertTrue(true);
}


/*test(VehicleControllerTest, test_registerDevice) {
  VehicleController vc;
  Device dev;
  assertTrue(vc.registerDevice(&dev));
}


test(VehicleControllerTest, test_registerDevice_doubleOccurence) {
  VehicleController vc;
  Device dev;
  vc.registerDevice(&dev);
  assertFalse(vc.registerDevice(&dev));
}*/


test(VehicleControllerTest, test_registerParameter_) {
  VehicleController vc;
  assertTrue(vc.registerParameter(&param));
}


test(VehicleControllerTest, test_registerParameter_paramBool) {
  VehicleController vc;
  assertTrue(vc.registerParameter(&paramBool));
}


test(VehicleControllerTest, test_registerParameter_paramInt) {
  VehicleController vc;
  assertTrue(vc.registerParameter(&paramInt));
}


test(VehicleControllerTest, test_registerParameter_paramDouble) {
  VehicleController vc;
  assertTrue(vc.registerParameter(&paramDouble));
}


test(VehicleControllerTest, test_registerParameter_doubleId) {
  VehicleController vc;
  vc.registerParameter(&param);
  assertFalse(vc.registerParameter(&param));
}


test(VehicleControllerTest, test_registerForValueChanged_idCorrect) {
  VehicleController vc;
  Device dev;
  int id = param.getId();
  vc.registerParameter(&param);
  assertTrue(vc.registerForValueChanged(&dev, id));
}


test(VehicleControllerTest, test_registerForValueChanged_idNotCorrect) {
  VehicleController vc;
  Device dev;
  assertFalse(vc.registerForValueChanged(&dev, 100));
}


test(VehicleControllerTest, test_unregisterForValueChanged_idCorrect) {
  VehicleController vc;
  Device dev;
  int id = param.getId();
  vc.registerParameter(&param);
  vc.registerForValueChanged(&dev, id);
  assertTrue(vc.unregisterForValueChanged(&dev, id));
}


test(VehicleControllerTest, test_unregisterForValueChanged_idNotCorrect) {
  VehicleController vc;
  Device dev;
  int id = param.getId();
  vc.registerParameter(&param);
  vc.registerForValueChanged(&dev, id);
  assertFalse(vc.unregisterForValueChanged(&dev, 100));
}


/*test(VehicleControllerTest, test_unregisterForAll_deviceRegistered) {
  VehicleController vc;
  Device dev;
  int id = param.getId();
  vc.registerParameter(&param);
  vc.registerForValueChanged(&dev, id);
  assertTrue(vc.unregisterForAll(&dev));
}


test(VehicleControllerTest, test_unregisterForAll_deviceRegisteredTwice) {
  VehicleController vc;
  Device dev;
  int id1 = param.getId();
  int id2 = paramBool.getId();
  vc.registerParameter(&param);
  vc.registerParameter(&paramBool);
  vc.registerForValueChanged(&dev, id1);
  vc.registerForValueChanged(&dev, id2);
  assertTrue(vc.unregisterForAll(&dev));
}


test(VehicleControllerTest, test_unregisterForAll_deviceNotRegistered) {
  VehicleController vc;
  Device dev;
  assertFalse(vc.unregisterForAll(&dev));
}*/


test(VehicleControllerTest, test_setBooleanValue_paramCorrect) {
  VehicleController vc;
  Device dev;
  vc.registerParameter(&paramBool);
  assertTrue(vc.setBooleanValue(&dev, &paramBool, true));
}


test(VehicleControllerTest, test_setIntegerValue_paramCorrect) {
  VehicleController vc;
  Device dev;
  vc.registerParameter(&paramInt);
  assertTrue(vc.setIntegerValue(&dev, &paramInt, 5));
}


test(VehicleControllerTest, test_setDoubleValue_paramCorrect) {
  VehicleController vc;
  Device dev;
  vc.registerParameter(&paramDouble);
  assertTrue(vc.setDoubleValue(&dev, &paramDouble, 0.1));
}


test(VehicleControllerTest, test_setBooleanValue_paramNotRegistered) {
  VehicleController vc;
  Device dev;
  assertFalse(vc.setBooleanValue(&dev, &paramBool, true));
}


test(VehicleControllerTest, test_setIntegerValue_paramNotRegistered) {
  VehicleController vc;
  Device dev;
  assertFalse(vc.setIntegerValue(&dev, &paramInt, 5));
}


test(VehicleControllerTest, test_setDoubleValue_paramNotRegistered) {
  VehicleController vc;
  Device dev;
  assertFalse(vc.setDoubleValue(&dev, &paramDouble, 0.4));
}


test(VehicleControllerTest, test_setBooleanValue_sameValueAsBefore) {
  VehicleController vc;
  Device dev;
  vc.registerParameter(&paramBool);
  vc.setBooleanValue(&dev, &paramBool, true);
  assertTrue(vc.setBooleanValue(&dev, &paramBool, true));
}


test(VehicleControllerTest, test_setIntegerValue_sameValueAsBefore) {
  VehicleController vc;
  Device dev;
  vc.registerParameter(&paramInt);
  vc.setIntegerValue(&dev, &paramInt, 5);
  assertTrue(vc.setIntegerValue(&dev, &paramInt, 5));
}


test(VehicleControllerTest, test_setDoubleValue_sameValueAsBefore) {
  VehicleController vc;
  Device dev;
  vc.registerParameter(&paramDouble);
  vc.setDoubleValue(&dev, &paramDouble, 0.6);
  assertTrue(vc.setDoubleValue(&dev, &paramDouble, 0.6));
}
