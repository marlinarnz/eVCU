#ifndef DEVICEPIN_H
#define DEVICEPIN_H

#include <Arduino.h>
#include "Device.h"


/** Base class for Devices that handle input pin events.
 *  Uses the Device base class and adds a task for input
 *  observation.
 */
class DevicePin : public Device
{
public:
  DevicePin(VehicleController* pController);
  ~DevicePin();
  TaskHandle_t getTaskHandle();

private:
  virtual void onPinInterrupt();                      // To be defined in derived class
  void onPinInterruptLoop(void* pvParameters);

protected:
  void startTasks(uint16_t stackSizeOnValueChanged=4096,
                  uint16_t stackSizeOnPinInterrupt=4096);
  static void startOnPinInterruptLoop(void* _this);
  TaskHandle_t m_taskHandleOnPinInterrupt;
};

#endif
