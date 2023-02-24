#ifndef DEVICEPIN_H
#define DEVICEPIN_H

#include <Arduino.h>
//#include <FunctionalInterrupt.h>
#include "Device.h"


/** Base class for Devices that handle input pin events.
 *  Uses the Device base class and adds a task for one input
 *  observation through an interrupt service routine (ISR). The
 *  ISR must be attached to the pin in the child class's `begin()`
 *  function by calling `attachISR()`.
 */
class DevicePin : public Device
{
public:
  DevicePin(VehicleController* pController, uint8_t pin, int debounce, int inputMode, int interruptMode);
  ~DevicePin();

private:
  virtual void onPinInterrupt();                      // To be defined in derived class
  void onPinInterruptLoop(void* pvParameters);
  //void ARDUINO_ISR_ATTR isr(void* arg);
  int m_debounce;
  long m_lastPinInterrupt;
  int m_inputMode;
  int m_interruptMode;

protected:
  void attachISR();
  void startTasks(uint16_t stackSizeOnValueChanged=configMINIMAL_STACK_SIZE*4,
                  uint16_t stackSizeOnPinInterrupt=4096);
  static void startOnPinInterruptLoop(void* _this);
  TaskHandle_t m_taskHandleOnPinInterrupt;
  uint8_t m_pin;
};

#endif
