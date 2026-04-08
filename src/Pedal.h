#ifndef PEDAL_H
#define PEDAL_H

#include <Arduino.h>
#include "DeviceLoop.h"
#include "SecuredLinkedListMap.h"


// Number of positions in the smoothen array
#define N_PREV_VALS 5


/** Class for pedals (throttle or brake).
 *  Use ESP32's ADC for analogue voltage measurement. Potentiometers or
 *  modern automotive PPAs return a voltage depending of pedal
 *  depression, usually based on a 5V input. A position map translates
 *  the measured voltage into a percentage value. The Pedal writes the
 *  position into the given Parameter instance (0 to 99.7 %) every given
 *  time interval.
 */
class Pedal : public DeviceLoop
{
public:
  Pedal(VehicleController* vc, uint8_t pin, int readInterval, ParameterDouble* pParam, SecuredLinkedListMap<double, double>* pMap, double vref, double dividerRatio, ParameterBool* pParamInhibit=nullptr);
  ~Pedal();
  void begin();
  void shutdown();
  
private:
  void onValueChanged(Parameter* pParamWithNewValue) {};
  void onLoop();
  float smoothen(float newPosition);
  float m_prevVals[N_PREV_VALS];
  double mapADC(int adc);
  ParameterDouble* m_pParam;
  ParameterBool* m_pParamInhibit;
  uint8_t m_pin;
  SecuredLinkedListMap<double, double>* m_pMap;
  double m_vref;
  double m_dividerRatio;
};

#endif
