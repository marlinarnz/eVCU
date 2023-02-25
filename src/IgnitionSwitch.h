#ifndef IGNITIONSWITCH_H
#define IGNITIONSWITCH_H

#include <Arduino.h>
#include "Constants.h"
#include "DevicePinMulti.h"


/** Class for ignition switches.
 *  Observes 3 pins to write the ignition switch position into a
 *  `ParameterInt` (`0`: off, `1`: acc, `2`: on, `3`: start).
 */
class IgnitionSwitch : public DevicePinMulti<3>
{
public:
  IgnitionSwitch(VehicleController* vc, uint8_t pins[3], int inputModes[3], ParameterInt* pParam, int debounce=SWITCH_DEBOUNCE_MS);
  ~IgnitionSwitch();
  void begin();
  void shutdown();
  
private:
  void onValueChanged(Parameter* pParamWithNewValue);
  void onPinInterrupt();
  int getSwitchPosition();
  ParameterInt* m_pParam;
};

#endif
