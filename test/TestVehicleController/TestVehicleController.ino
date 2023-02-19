#line 2 "TestVehicleController.ino"

#include <AUnitVerbose.h>
#include <Parameter.h>
#include <VehicleController.h>
#include <Device.h>

using aunit::TestRunner;


Parameter param(0);
ParameterBool paramBool(1);
ParameterInt paramInt(2);
ParameterDouble paramDouble(3);

// Create a mock device which inherits from abstract class Device
class DeviceMock : public Device
{
public:
  DeviceMock(VehicleController* vc) : Device(vc) {};
  void begin() { this->startTasks(8000); PRINT("called begin") };
  void shutdown() { PRINT("called shutdown") };
private:
  void onValueChanged(Parameter* pParamWithNewValue) { PRINT("called onValueChanged") };
};


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


test(VehicleControllerTest, test_registerParameter_) {
  VehicleController vc;
  assertTrue(vc.registerParameter(0));
}


test(VehicleControllerTest, test_registerParameter_paramBool) {
  VehicleController vc;
  assertTrue(vc.registerParameter(1));
}


test(VehicleControllerTest, test_registerParameter_paramInt) {
  VehicleController vc;
  assertTrue(vc.registerParameter(2));
}


test(VehicleControllerTest, test_registerParameter_paramDouble) {
  VehicleController vc;
  assertTrue(vc.registerParameter(3));
}


test(VehicleControllerTest, test_registerParameter_doubleId) {
  VehicleController vc;
  vc.registerParameter(0);
  assertFalse(vc.registerParameter(0));
}


test(VehicleControllerTest, test_registerForValueChanged_idCorrect) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  int id = param.getId();
  vc.registerParameter(0);
  assertTrue(vc.registerForValueChanged(&dev, id));
}


// DEPRECTED
//test(VehicleControllerTest, test_registerForValueChanged_idNotCorrect) {
//  VehicleController vc;
//  DeviceMock dev = DeviceMock(&vc);
//  assertFalse(vc.registerForValueChanged(&dev, 100));
//}


test(VehicleControllerTest, test_unregisterForValueChanged_idCorrect) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  int id = param.getId();
  vc.registerParameter(0);
  vc.registerForValueChanged(&dev, id);
  assertTrue(vc.unregisterForValueChanged(&dev, id));
}


test(VehicleControllerTest, test_unregisterForValueChanged_idNotCorrect) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  int id = param.getId();
  vc.registerParameter(0);
  vc.registerForValueChanged(&dev, id);
  assertFalse(vc.unregisterForValueChanged(&dev, 100));
}


test(VehicleControllerTest, test_setBooleanValue_paramCorrect) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  vc.registerParameter(1);
  assertTrue(vc.setBooleanValue(&dev, &paramBool, true));
}


test(VehicleControllerTest, test_setIntegerValue_paramCorrect) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  vc.registerParameter(2);
  assertTrue(vc.setIntegerValue(&dev, &paramInt, 5));
}


test(VehicleControllerTest, test_setDoubleValue_paramCorrect) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  vc.registerParameter(3);
  assertTrue(vc.setDoubleValue(&dev, &paramDouble, 0.1));
}


test(VehicleControllerTest, test_setBooleanValue_paramNotRegistered) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  assertFalse(vc.setBooleanValue(&dev, &paramBool, true));
}


test(VehicleControllerTest, test_setIntegerValue_paramNotRegistered) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  assertFalse(vc.setIntegerValue(&dev, &paramInt, 5));
}


test(VehicleControllerTest, test_setDoubleValue_paramNotRegistered) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  assertFalse(vc.setDoubleValue(&dev, &paramDouble, 0.4));
}


test(VehicleControllerTest, test_setBooleanValue_sameValueAsBefore) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  vc.registerParameter(1);
  vc.setBooleanValue(&dev, &paramBool, true);
  assertFalse(vc.setBooleanValue(&dev, &paramBool, true));
}


test(VehicleControllerTest, test_setIntegerValue_sameValueAsBefore) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  vc.registerParameter(2);
  vc.setIntegerValue(&dev, &paramInt, 5);
  assertFalse(vc.setIntegerValue(&dev, &paramInt, 5));
}


test(VehicleControllerTest, test_setDoubleValue_sameValueAsBefore) {
  VehicleController vc;
  DeviceMock dev = DeviceMock(&vc);
  vc.registerParameter(3);
  vc.setDoubleValue(&dev, &paramDouble, 0.6);
  assertFalse(vc.setDoubleValue(&dev, &paramDouble, 0.6));
}
