#include "Contactors.h"


// Define default fault parameters
ParameterBool mainRelayFaultDefault(990);
ParameterBool auxRelayFaultDefault(991);


/** The constructor saves all relevant information as members.
 *  Inits the pin modes.
 *  @param vc pointer to the VehicleController instance
 *  @param pinMain pin for the main contactor relay
 *  @param pinAux pin for the precharge contactor relay
 *  @param prechargeTime time difference in ms between precharge
 *                       contactor and main contactor closed
 *  @param pParamMainConn `Parameter` to save main contactor state, 
 *                        `true` is connected, `false` dicsonnected
 *  @param pParamAuxConn `Parameter` to save precharge contactor state, 
 *                       `true` is connected, `false` dicsonnected
 *  @param pParamIgnition `Parameter` giving the ignition switch state: 
 *                        `0`: off, `1`: acc, `2`: on, `3`: start
 *  @param pParamVehicleReady `Parameter` giving the vehicle readiness
 *  @param keyPositionStart (optional) the key position at which the
 *                          contactors are closed, default `2`
 *  @param loopTime (optional) time for status checks in ms
 *  @param pinCheckMain (optional) pin for the main contactor fault line
 *  @param pinCheckAux (optional) pin for the precharge contactor fault line
 *  @param pParamMainFault (only required when pinCheckMain given)
 *                         `Parameter` to report main contactor faults
 *  @param pParamAuxFault (only required when pinCheckAux given)
 *                        `Parameter` to report precharge contactor faults
 *  @param onIsHigh (optional) the physical pin value for the relay pins,
 *                  set to `false` to switch relais at ´LOW`
 */
Contactors::Contactors(VehicleController* vc,
                       uint8_t pinMain,
                       uint8_t pinAux,
                       uint16_t prechargeTime,
                       ParameterBool* pParamMainConn,
                       ParameterBool* pParamAuxConn,
                       ParameterInt* pParamIgnition,
                       ParameterBool* pParamVehicleReady,
                       uint8_t keyPositionStart,
                       uint16_t loopTime,
                       uint8_t pinCheckMain,
                       uint8_t pinCheckAux,
                       ParameterBool* pParamMainFault,
                       ParameterBool* pParamAuxFault,
                       bool onIsHigh)
  : DeviceLoop(vc, loopTime),
    m_pinMain(pinMain), m_pinAux(pinAux), m_prechargeTime(prechargeTime),
    m_pParamMainConn(pParamMainConn), m_pParamAuxConn(pParamAuxConn),
    m_pParamIgnition(pParamIgnition), m_pParamVehicleReady(pParamVehicleReady),
    m_keyPositionStart(keyPositionStart),
    m_pinCheckMain(pinCheckMain), m_pinCheckAux(pinCheckAux),
    m_pParamMainFault(pParamMainFault), m_pParamAuxFault(pParamAuxFault),
    m_precharging(0), m_prechargeStartTime(0)
{
  // Set the digital pin mode, when contactor is closed
  if (onIsHigh) {
    m_on = HIGH;
    m_off = LOW;
  } else {
    m_on = LOW;
    m_off = HIGH;
  }
  
  // Set contactor pins as output
  pinMode(pinMain, OUTPUT);
  pinMode(pinAux, OUTPUT);

  // Set the check pins as input, if given
  if (pinCheckMain != 0) {
    pinMode(pinCheckMain, INPUT_PULLDOWN);
  }
  if (pinCheckAux != 0) {
    pinMode(pinCheckAux, INPUT_PULLDOWN);
  }
}


/** The destructor does nothing.
 */
Contactors::~Contactors()
{}


/** Start tasks, set default values, subscribe to parameters.
 */
void Contactors::begin()
{
  // Init the pin states
  digitalWrite(m_pinMain, m_off);
  digitalWrite(m_pinAux, m_off);
  
  // Start tasks
  this->startTasks(4096, 4096);

  // Subscribe to parameter changes
  this->registerForValueChanged(m_pParamIgnition->getId());
  this->registerForValueChanged(m_pParamVehicleReady->getId());

  // Init the contactor states
  this->setBooleanValue(m_pParamMainConn, false);
  this->setBooleanValue(m_pParamAuxConn, false);
  this->setBooleanValue(m_pParamMainFault, false);
  this->setBooleanValue(m_pParamAuxFault, false);
}


/** End operation.
 */
void Contactors::shutdown()
{
  this->unregisterForValueChanged(m_pParamIgnition->getId());
  this->unregisterForValueChanged(m_pParamVehicleReady->getId());
  digitalWrite(m_pinMain, m_off);
  digitalWrite(m_pinAux, m_off);
}


/** Switch contactors off.
 *  Other devices can call this functions in case of emergency.
 */
void Contactors::emergencyPowerOff()
{
  // Switch contactors off
  digitalWrite(m_pinMain, m_off);
  digitalWrite(m_pinAux, m_off);
  this->setBooleanValue(m_pParamMainConn, false);
  this->setBooleanValue(m_pParamAuxConn, false);
}


/** Reacts to ignition switch and vehicle readiness.
 *  Starts precharging and opens contactors, respectively.
 */
void Contactors::onValueChanged(Parameter* pParam)
{
  if (pParam->getId() == m_pParamIgnition->getId()) {
    if (m_pParamIgnition->getVal() == m_keyPositionStart
        && m_pParamVehicleReady->getVal()
		&& !m_pParamMainConn->getVal())
    {
      // start the precharge process
      digitalWrite(m_pinAux, m_on);
      this->setBooleanValue(m_pParamAuxConn, true);
      m_precharging = true;
      m_prechargeStartTime = millis();
    }
    else if (m_pParamIgnition->getVal() < m_keyPositionStart) {
      // close the contactors
      digitalWrite(m_pinMain, m_off);
      digitalWrite(m_pinAux, m_off);
      this->setBooleanValue(m_pParamMainConn, false);
      this->setBooleanValue(m_pParamAuxConn, false);
    }
  }
  
  else if (pParam->getId() == m_pParamVehicleReady->getId()) {
    if (!m_pParamVehicleReady->getVal()) {
      // close the contactors
      digitalWrite(m_pinMain, m_off);
      digitalWrite(m_pinAux, m_off);
      this->setBooleanValue(m_pParamMainConn, false);
      this->setBooleanValue(m_pParamAuxConn, false);
    }
  }
}


/** Observe eprecharge time and fault check lines.
 *  Once the precharge resistor has connected and the vehicle is
 *  precharging, the loop waits until the precharge time is over
 *  and connects the main contactor. If fault check pins were given
 *  in the constructor, these are checked for correct functionality.
 */
void Contactors::onLoop()
{
  // Check precharging
  if (m_precharging && millis() - m_prechargeStartTime > m_prechargeTime) {
    m_precharging = false;
    digitalWrite(m_pinMain, m_on);
    this->setBooleanValue(m_pParamMainConn, true);
    digitalWrite(m_pinAux, m_off);
    this->setBooleanValue(m_pParamAuxConn, false);
  }

  // Check the check pins as input, if given
  if (m_pinCheckMain != 0) {
    uint8_t state = digitalRead(m_pinCheckMain);
    if (state == m_on && m_pParamMainConn->getVal() == false) {
      // Turn pin off and notify fault
      digitalWrite(m_pinMain, m_off);
      this->setBooleanValue(m_pParamMainFault, true);
    }
    else if (state == m_off && m_pParamMainConn->getVal() == true) {
      // Turn pin off and notify fault
      digitalWrite(m_pinMain, m_off);
      this->setBooleanValue(m_pParamMainFault, true);
    }
    else this->setBooleanValue(m_pParamMainFault, false);
  }
  if (m_pinCheckAux != 0) {
    uint8_t state = digitalRead(m_pinCheckAux);
    if (state == m_on && m_pParamAuxConn->getVal() == false) {
      // Turn pin off and notify fault
      digitalWrite(m_pinAux, m_off);
      this->setBooleanValue(m_pParamAuxFault, true);
    }
    else if (state == m_off && m_pParamAuxConn->getVal() == true) {
      // Turn pin off and notify fault
      digitalWrite(m_pinAux, m_off);
      this->setBooleanValue(m_pParamAuxFault, true);
    }
    else this->setBooleanValue(m_pParamAuxFault, false);
  }
}
