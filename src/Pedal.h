#ifndef PEDAL_H
#define PEDAL_H

#include <Arduino.h>
#include "DeviceLoop.h"


// Number of positions in the smoothen array
#define N_PREV_VALS 5


/** Class for pedals (throttle or brake).
 *  Use GPIO 32 or 33 for analog position measure via ESP32's ADC.
 *  The Pedal writes the position into the given Parameter instance
 *  (0 to 99.7 %) every given time interval.
 */
class Pedal : public DeviceLoop
{
public:
  Pedal(VehicleController* vc, uint8_t pin, int readInterval, ParameterDouble* pParam);
  ~Pedal();
  void begin();
  void shutdown();
  
private:
  void onValueChanged(Parameter* pParamWithNewValue) {};
  void onLoop();
  float smoothen(float newPosition);
  float m_prevVals[N_PREV_VALS];
  ParameterDouble* m_pParam;
  uint8_t m_pin;
};

#endif
