#ifndef DEVICELOOP_H
#define DEVICELOOP_H

#include <Arduino.h>
#include "Device.h"


/** Base class for Devices that act in a loop.
 *  Uses the Device base class and adds a task for a loop that is
 *  constantly running. Useful for analog value sampling, as the
 *  ESP32 does not support interrupts for analog voltage change.
 */
class DeviceLoop : public Device
{
public:
  DeviceLoop(VehicleController* pController, int loopTime);
  ~DeviceLoop();

private:
  virtual void onLoop();                      // To be defined in derived class
  void onLoopLoop(void* pvParameters);
  int m_loopTime;

protected:
  void startTasks(uint16_t stackSizeOnValueChanged=configMINIMAL_STACK_SIZE*4,
                  uint16_t stackSizeLoop=4096,
                  uint8_t core=1);
  static void startLoopLoop(void* _this);
  TaskHandle_t m_taskHandleLoop;
};

#endif
