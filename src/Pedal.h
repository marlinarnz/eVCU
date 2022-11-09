#ifndef PEDAL_H
#define PEDAL_H

#include <Arduino.h>
#include "DevicePin.h"


// Number of positions in the smoothen array
#define N_PREV_VALS 10


/** Class for pedals (throttle or brake).
 *  Use GPIO 32 or 33 for analog position measure via ESP32's ADC.
 *  An ISR triggers reading the analog pin only when the given
 *  interval since the last read has passed. The Pedal writes
 *  the position into the given Parameter instance (0 to 99.7 %).
 */
class Pedal : public DevicePin
{
public:
  Pedal(VehicleController* vc, uint8_t pin, int readInterval, ParameterDouble* pParam);
  ~Pedal();
  void begin();
  void shutdown();
  
private:
  //void onValueChanged(Parameter* pParamWithNewValue);
  void onPinInterrupt();
  float smoothen(float newPosition);
  float prevVals[N_PREV_VALS];
  ParameterDouble* m_pParam;
};

#endif
