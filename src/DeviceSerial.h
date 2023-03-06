#ifndef DEVICESERIAL_H
#define DEVICESERIAL_H

#include <Arduino.h>
#include "Device.h"


/** Base class for Devices that handle serial input events.
 *  Uses the Device base class and adds a task for input
 *  observation. Can be used for e.g. CAN bus, UART, SPI.
 */
class DeviceSerial : public Device
{
public:
  DeviceSerial(VehicleController* pController);
  ~DeviceSerial();

private:
  virtual void waitForSerialEvent();                  // To be defined in derived class
  void onSerialEventLoop(void* pvParameters);

protected:
  virtual void startTasks(uint16_t stackSizeOnValueChanged=4096,
                          uint16_t stackSizeOnSerialEvent=4096,
                          uint8_t core=1);
  static void startOnSerialEventLoop(void* _this);
  TaskHandle_t m_taskHandleOnSerialEvent;
};

#endif
