#ifndef CONTACTORS_H
#define CONTACTORS_H

#include <Arduino.h>
#include "DeviceLoop.h"
#include "Parameter.h"


// Declare default fault parameters
extern ParameterBool mainRelayFaultDefault;
extern ParameterBool auxRelayFaultDefault;


/** Manages the contactors.
 *  Connects the auxiliary contactor at vehicle start, given an
 *  ignition switch parameter, and the main contactor after a given
 *  time interval. Optionally checks the physical contactor state,
 *  if pin(s) for fault lines are given to the constructor.
 */
class Contactors : public DeviceLoop
{
public:
  Contactors(VehicleController* vc,
             uint8_t pinMain, uint8_t pinAux, uint16_t prechargeTime,
             ParameterBool* pParamMainConn, ParameterBool* pParamAuxConn,
             ParameterInt* pParamIgnition, ParameterBool* pParamVehicleReady,
             uint8_t keyPositionStart=2, bool onIsHigh=true, uint16_t loopTime=100,
             uint8_t pinCheckMain=0, uint8_t pinCheckAux=0,
             ParameterBool* pParamMainFault=&mainRelayFaultDefault,
             ParameterBool* pParamAuxFault=&auxRelayFaultDefault);
  ~Contactors();
  void begin();
  void shutdown();
  void emergencyPowerOff();

private:
  void onValueChanged(Parameter* pParam);
  void onLoop();
  uint8_t m_pinMain;
  uint8_t m_pinAux;
  uint16_t m_prechargeTime;
  ParameterBool* m_pParamMainConn;
  ParameterBool* m_pParamAuxConn;
  ParameterInt* m_pParamIgnition;
  ParameterBool* m_pParamVehicleReady;
  uint8_t m_keyPositionStart;
  uint8_t m_pinCheckMain;
  uint8_t m_pinCheckAux;
  ParameterBool* m_pParamMainFault;
  ParameterBool* m_pParamAuxFault;
  bool m_precharging;
  long m_prechargeStartTime;
  uint8_t m_on;
  uint8_t m_off;
};

#endif
