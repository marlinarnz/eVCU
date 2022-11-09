#ifndef SWITCH_H
#define SWITCH_H

#include <Arduino.h>
#include "Constants.h"
#include "DevicePin.h"


/** Class for all simple switch objects.
 *  Manages one digital switch at one GPIO pin and writes into a
 *  `ParameterBool` (`LOW`=`false`, `HIGH`=`true`).
 */
class Switch : public DevicePin
{
public:
  Switch(VehicleController* vc, uint8_t pin, int inputMode, ParameterBool* pParam, int debounce=SWITCH_DEBOUNCE_MS);
  ~Switch();
  void begin();
  void shutdown();
  
private:
  void onValueChanged(Parameter* pParamWithNewValue);
  void onPinInterrupt();
  ParameterBool* m_pParam;
};

#endif
