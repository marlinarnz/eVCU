#include "IgnitionSwitch.h"


/** The constructor saves all relevant information as members.
 *  @param vc pointer to the VehicleController instance
 *  @param pins array of 3 GPIO pins to use as input in this order:
 *              acc (KL75), on (KL15), start (KL50)
 *  @param inputModes array of 3 modes for the Arduino function `pinMode()`.
 *                    Can be `INPUT`; `INPUT_PULLUP`; `INPUT_PULLDOWN`
 *  @param pParam pointer to the ParameterInt instance that
 *                shall inform other Devices about switch position
 *  @param debounce minimum time between two pin interactions in ms.
 *                  Defaults to macro `SWITCH_DEBOUNCE_MS`
 */
IgnitionSwitch::IgnitionSwitch(VehicleController* vc, uint8_t pins[3], int inputModes[3], ParameterInt* pParam, int debounce)
  : DevicePinMulti<3>(vc, pins, debounce, inputModes, CHANGE),
    m_pParam(pParam)
{}


/** The destructor does nothing.
 */
IgnitionSwitch::~IgnitionSwitch()
{}


/** Start tasks.
 *  Pin mode parameters were already handled in the parent class
 *  constructor, but the interrupt must be attached at begin.
 *  Init the switch position to the physical value.
 */
void IgnitionSwitch::begin()
{
  // Attach the interrupt on the pins given in the constructor
  DevicePinMulti<3>::attachISR();
  // Start tasks
  this->startTasks(4096, 8192);
  // Init the switch position
  this->setIntegerValue(m_pParam, getSwitchPosition());
}


/** At shutdown, nothing happens.
 */
void IgnitionSwitch::shutdown()
{}


/** Notify other Devices about the switch status change.
 */
void IgnitionSwitch::onPinInterrupt()
{
  this->setIntegerValue(m_pParam, getSwitchPosition());
}


/** Figure out the position of the ignition switch.
 *  Start, if KL50 is high, on, if KL15 is high, acc, if KL75 is
 *  high, else off
 *  @return integer position of the ignition switch:
 *          `0`: off, `1`: acc, `2`: on, `3`: start
 */
int IgnitionSwitch::getSwitchPosition()
{
  if (digitalRead(m_pins[2]) == HIGH) {
    return 3;
  }
  else if (digitalRead(m_pins[1]) == HIGH) {
    return 2;
  }
  else if (digitalRead(m_pins[0]) == HIGH) {
    return 1;
  }
  return 0;
}


/** The IgnitionSwitch Device is not interested in other Devices.
 */
void IgnitionSwitch::onValueChanged(Parameter* pParamWithNewValue)
{}
