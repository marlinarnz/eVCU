#include "Switch.h"


/** The constructor saves all relevant information as members.
 *  @param vc pointer to the VehicleController instance
 *  @param pin GPIO pin to use as input for the switch
 *  @param inputMode mode for the Arduino function `pinMode()`.
 *                   Can be `INPUT`; `INPUT_PULLUP`; `INPUT_PULLDOWN`
 *  @param pParam pointer to the ParameterBool instance that
 *                shall inform other Devices about switch actions
 */
Switch::Switch(VehicleController* vc, uint8_t pin, int inputMode, ParameterBool* pParam)
  : DevicePin(vc), m_pin(pin), m_pinMode(inputMode), m_pParam(pParam)
{}


/** The destructor calls shutdown.
 */
Switch::~Switch()
{
  this->shutdown();
}


/** Start tasks and pins.
 *  Choose the `onValueChangedLoop` task as small as possible while
 *  the other task should accomodate all Devices interested. Pin
 *  mode parameters were given in the constructor.
 */
void Switch::begin()
{
  this->startTasks(2048, 8192);
  pinMode(this->m_pin, this->m_pinMode);
}


/** Detach the interrupt on the switch pin 
 */
void Switch::shutdown()
{
  detachInterrupt(m_pin);
}


/** Notify other Devices about the switch status change.
 */
void Switch::onPinInterrupt()
{
  this->setBooleanValue(m_pParam, digitalRead(m_pin));
}


/** The Switch Device is not interested in other Devices.
 */
void Switch::onValueChanged(Parameter* pParamWithNewValue)
{}
