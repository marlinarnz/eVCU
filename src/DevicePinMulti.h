#ifndef DEVICEPINMULTI_H
#define DEVICEPINMULTI_H

#include <Arduino.h>
#include "DevicePin.h"


/** Base class for Devices that handle multiple input pin events.
 *  Uses the DevicePin base class and add more pins to the same
 *  interrupt service routing (ISR). It must be attached to the pins
 *  in the child class's `begin()` function by calling `attachISR()`.
 *  Instantiate this class in the C++ template manner:
 *  `DevicePinMulti<numerOfPins>(what goes into the constructor)`
 */
template <int N>
class DevicePinMulti : public DevicePin
{
public:
  DevicePinMulti(VehicleController* pController, uint8_t pins[N], int debounce, int inputModes[N], int interruptMode);
  ~DevicePinMulti();

private:
  int m_inputModes[N];

protected:
  void attachISR();
  uint8_t m_pins[N];
};


/** The constructor attaches the interrupt.
 *  @param pController: VehicleController instance passed to Device
 *  @param pins: array of the GPIO pins for the interrupt
 *  @param debounce: minimum millis between two interrupt yields
 *  @param inputModes: array of INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 *  @param interruptMode: at pin voltage LOW, CHANGE, RISING, FALLING
 */
template <int N>
DevicePinMulti<N>::DevicePinMulti(VehicleController* pController, uint8_t pins[N], int debounce, int inputModes[N], int interruptMode)
  : DevicePin(pController, pins[0], debounce, inputModes[0], interruptMode)
{
  for (int i=0; i<N; i++) {
    m_pins[i] = pins[i];
    m_inputModes[i] = inputModes[i];
  }
}


/** The destructor detaches the interrupt.
 *  The first pin is already detached in the parent destructor
 */
template <int N>
DevicePinMulti<N>::~DevicePinMulti()
{
  for (int i=1; i<N; i++) {
    detachInterrupt(m_pins[i]);
  }
}


/** Attach the ISR to each pin.
 *  Must be called in the child class's `begin()` function. First
 *  sets the pin mode, then attaches the ISR.
 */
template <int N>
void DevicePinMulti<N>::attachISR()
{
  for (int i=0; i<N; i++) {
    pinMode(m_pins[i], m_inputModes[i]);
    attachInterruptArg(m_pins[i], isrPin, &m_taskHandleOnPinInterrupt, m_interruptMode);
  }
}


#endif
