#include "Switch.h"


/** The constructor saves all relevant information as members.
 *  @param vc pointer to the VehicleController instance
 *  @param pin GPIO pin to use as input for the switch
 *  @param inputMode mode for the Arduino function `pinMode()`.
 *                   Can be `INPUT`; `INPUT_PULLUP`; `INPUT_PULLDOWN`
 *  @param pParam pointer to the ParameterBool instance that
 *                shall inform other Devices about switch actions
 *  @param debounce minimum time between two pin interactions in ms.
 *                  Defaults to macro `SWITCH_DEBOUNCE_MS`
 */
Switch::Switch(VehicleController* vc, uint8_t pin, int inputMode, ParameterBool* pParam, int debounce)
  : DevicePin(vc, pin, debounce, inputMode, CHANGE),
    m_pParam(pParam)
{}


/** The destructor does nothing.
 */
Switch::~Switch()
{}


/** Start tasks.
 *  Choose the `onValueChangedLoop` task as small as possible while
 *  the other task should accomodate all Devices interested. Pin
 *  mode parameters were already handled in the parent class
 *  constructor.
 */
void Switch::begin()
{
  this->startTasks(4096, 8192);
}


/** At shutdown, nothing happens.
 */
void Switch::shutdown()
{}


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
