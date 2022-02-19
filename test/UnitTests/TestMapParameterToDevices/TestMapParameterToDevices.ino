#line 2 "TestMapParameterToDevice.ino"

#include <AUnitVerbose.h>
#include "MapParameterToDevices.h"
#include "Parameter.h"
#include "SecuredLinkedList.h"
#include "Device.h"

using aunit::TestRunner;


Parameter param(0);
ParameterBool paramBool(1);
ParameterInt paramInt(2);
ParameterDouble paramDouble(3);


// Prepare the serial port, include/exclude tests
void setup() {
    Serial.begin(9600);
    delay(200);
    TestRunner::setTimeout(120);
    //TestRunner::exclude("*");
    //TestRunner::include("*");
}

// Call the test runner in loop
void loop() {
    TestRunner::run();
}


test(MapParameterToDevicesTest, test_constructor) {
  // Create instances within a scope, which calls the destructor at the end
  if(true) {
    MapParameterToDevices m0 = MapParameterToDevices();
    MapParameterToDevices* m1 = new MapParameterToDevices();
    delete m1;
  }
  assertTrue(true);
}


test(MapParameterToDevicesTest, test_addParameterToMap_paramOnHeap) {
  //MapParameterToDevices* m1 = new MapParameterToDevices(); // won't push if created on heap
  MapParameterToDevices m = MapParameterToDevices();
  Parameter* param1 = new Parameter(55);
  bool result = m.addParameterToMap(param1);
  //delete m1;
  assertTrue(result);
}


test(MapParameterToDevicesTest, test_addParameterToMap_paramBool) {
  MapParameterToDevices m = MapParameterToDevices();
  assertTrue(m.addParameterToMap(&paramBool));
}


test(MapParameterToDevicesTest, test_addParameterToMap_paramInt) {
  MapParameterToDevices m = MapParameterToDevices();
  assertTrue(m.addParameterToMap(&paramInt));
}


test(MapParameterToDevicesTest, test_addParameterToMap_paramDouble) {
  MapParameterToDevices m = MapParameterToDevices();
  assertTrue(m.addParameterToMap(&paramDouble));
}


test(MapParameterToDevicesTest, test_addParameterToMap_alreadyAdded) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&param);
  assertFalse(m.addParameterToMap(&param));
}


test(MapParameterToDevicesTest, test_registerForValueChanged_idCorrect) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&param);
  int id = param.getId();
  Device dev;
  assertTrue(m.registerForValueChanged(&dev, id));
}


test(MapParameterToDevicesTest, test_registerForValueChanged_idNotCorrect) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&param);
  int id = 100;
  Device dev;
  assertFalse(m.registerForValueChanged(&dev, id));
}


test(MapParameterToDevicesTest, test_registerForValueChanged_alreadyAdded) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&param);
  int id = param.getId();
  Device dev;
  m.registerForValueChanged(&dev, id);
  assertFalse(m.registerForValueChanged(&dev, id));
}


test(MapParameterToDevicesTest, test_unregisterForValueChanged_idCorrect) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&param);
  int id = param.getId();
  Device dev;
  m.registerForValueChanged(&dev, id);
  assertTrue(m.unregisterForValueChanged(&dev, id));
}


test(MapParameterToDevicesTest, test_unregisterForValueChanged_idNotCorrect) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&param);
  int id = param.getId();
  Device dev;
  m.registerForValueChanged(&dev, id);
  assertFalse(m.unregisterForValueChanged(&dev, 100));
}


/*test(MapParameterToDevicesTest, test_unregisterDevice_deviceRegisteredMiddle) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&paramInt);
  int id = paramInt.getId();
  Device dev0;
  Device dev1;
  Device dev2;
  m.registerForValueChanged(&dev0, id);
  m.registerForValueChanged(&dev1, id);
  m.registerForValueChanged(&dev2, id);
  assertTrue(m.unregisterDevice(&dev1));
}


test(MapParameterToDevicesTest, test_unregisterDevice_deviceRegisteredBottom) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&paramBool);
  int id = paramBool.getId();
  Device dev0;
  Device dev1;
  Device dev2;
  m.registerForValueChanged(&dev0, id);
  m.registerForValueChanged(&dev1, id);
  m.registerForValueChanged(&dev2, id);
  assertTrue(m.unregisterDevice(&dev0));
}


test(MapParameterToDevicesTest, test_unregisterDevice_deviceRegisteredTop) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&paramDouble);
  int id = paramDouble.getId();
  Device dev0;
  Device dev1;
  Device dev2;
  m.registerForValueChanged(&dev0, id);
  m.registerForValueChanged(&dev1, id);
  m.registerForValueChanged(&dev2, id);
  assertTrue(m.unregisterDevice(&dev2));
}


test(MapParameterToDevicesTest, test_unregisterDevice_deviceNotRegistered) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&param);
  int id = param.getId();
  Device dev;
  assertFalse(m.unregisterDevice(&dev));
}*/


test(MapParameterToDevicesTest, test_getRegisteredDevices_idCorrectWithDevice) {
  MapParameterToDevices m = MapParameterToDevices();
  Parameter* param1 = new Parameter(555);
  m.addParameterToMap(param1);
  int id = param1->getId();
  Device dev0;
  Device dev1;
  Device dev2;
  m.registerForValueChanged(&dev0, id);
  m.registerForValueChanged(&dev1, id);
  m.registerForValueChanged(&dev2, id);
  SecuredLinkedList<Device*>* devList = m.getRegisteredDevices(id);
  unsigned int len = devList->size();
  unsigned int correctLen = 3;
  assertEqual(len, correctLen);
}


test(MapParameterToDevicesTest, test_getRegisteredDevices_idNotCorrectWithDevice) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&param);
  int id = param.getId();
  Device dev;
  m.registerForValueChanged(&dev, id);
  SecuredLinkedList<Device*>* devList = m.getRegisteredDevices(100);
  assertEqual(devList, NULL);
}


test(MapParameterToDevicesTest, test_getRegisteredDevices_idCorrectWithoutDevice) {
  MapParameterToDevices m = MapParameterToDevices();
  Parameter* param1 = new Parameter(66);
  m.addParameterToMap(param1);
  int id = param1->getId();
  SecuredLinkedList<Device*>* devList = m.getRegisteredDevices(id);
  unsigned int len = devList->size();
  unsigned int correctLen = 0;
  assertEqual(len, correctLen);
}


test(MapParameterToDevicesTest, test_getRegisteredDevices_idNotCorrectWithoutDevice) {
  MapParameterToDevices m = MapParameterToDevices();
  m.addParameterToMap(&param);
  SecuredLinkedList<Device*>* devList = m.getRegisteredDevices(101);
  assertEqual(devList, NULL);
}
