#ifndef SWITCH_H
#define SWITCH_H

#include <Arduino.h>
#include "DevicePin.h"


/** Class for all simple switch objects.
 *  Manages one boolean switch at one GPIO pin defined at
 *  instantiation. The interrupt service routine (ISR), that
 *  triggers the switch event in the device, must be defined
 *  in the main file after object instantiation like so:
 *  @code
 *  volatile long lastPinInterrupt = 0;
 *  void IRAM_ATTR pinInterrupt<your pin here>() {
 *    if(millis() - lastPinInterrupt > SWITCH_DEBOUNCE_MS) {
 *      lastPinInterrupt = millis();
 *      // Notify the object's task
 *      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *      vTaskNotifyGiveFromISR(devOne.getTaskHandle(),
 *                             &xHigherPriorityTaskWoken);
 *      // Trigger a context change
 *      if(xHigherPriorityTaskWoken) {
 *        portYIELD_FROM_ISR();
 *      }
 *    }
 *  }
 *  @endcode
 *  Moreover, the setup section must attach the ISR after
 *  the Switch object's `begin()` function was called:
 *  @code
 *  attachInterrupt(<your pin here>, pinInterrupt<your pin here>, CHANGE);
 *  @endcode
 *  
 *  TODO: Useful concepts to allow ISRs as member functions:
 *  https://forum.arduino.cc/t/class-member-function-as-an-interrupt-service-routine/104337
 */
class Switch : public DevicePin
{
public:
  Switch(VehicleController* vc, uint8_t pin, int inputMode, ParameterBool* pParam);
  ~Switch();
  void begin();
  void shutdown();
  
private:
  void onValueChanged(Parameter* pParamWithNewValue);
  void onPinInterrupt();
  uint8_t m_pin;
  int m_pinMode;
  ParameterBool* m_pParam;
};

#endif
