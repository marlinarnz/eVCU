#include "Pedal.h"


#define ADC_WIDTH 9
#define ADC_RESOLUTION 511


/** The constructor configures the analog pin.
 *  @param vc pointer to the VehicleController instance
 *  @param pin analog GPIO pin hooked up to the pedal
 *  @param readInterval minimum time in ms between two position reads
 *  @param pParam pointer to the ParameterDouble instance that
 *                informs other Devices about pedal position in %
 */
Pedal::Pedal(VehicleController* vc, uint8_t pin, int readInterval, ParameterDouble* pParam)
  : DevicePin(vc, pin, readInterval, INPUT, CHANGE),
    m_pParam(pParam), prevVals{}
{
  // Good guidance through ADC on ESP32 and Arduino:
  // https://deepbluembedded.com/esp32-adc-tutorial-read-analog-voltage-arduino/
  // Attach the pin to ADC and clear previous settings
  //adcAttachPin(pin);
  // Set the pin's suggested voltage range (11 ~ full range)
  analogSetPinAttenuation(pin, ADC_11db);
  // Set the ADC resolution in bits (9=511 to 12=4095)
  analogSetWidth(ADC_WIDTH);
}


/** The destructor does nothing.
 */
Pedal::~Pedal()
{}


/** Start tasks.
 *  Choose the `onValueChangedLoop` task as small as possible while
 *  the other task should accomodate all Devices interested. Pin
 *  mode parameters were already handled in the parent class
 *  constructor.
 */
void Pedal::begin()
{
  this->startTasks(4096, 8192);
}


/** At shutdown, nothing happens.
 */
void Pedal::shutdown()
{}


/** Change the pedal position accordingly.
 *  Sets a new percentage value in the Parameter.
 */
void Pedal::onPinInterrupt()
{
  // Calculate the value
  double position = (analogRead(m_pin) / ADC_RESOLUTION * 99.7);
  // Set the value
  this->setDoubleValue(m_pParam, position);
}


/** Smoothens the pedal position.
 *  Takes the floating average of the n previous measurements where n
 *  is defined by constant `N_PREV_VALS`.
 *  @param newPosition: value to be smoothened
 *  @return float smoothened value
 */
float Pedal::smoothen(float newPosition)
{
  // Compute the floating average
  float sum = 0;
  for(byte i=0; i<N_PREV_VALS; i++) {
    sum += prevVals[i];
  }
  float positionSmoothed = (newPosition + sum) / (N_PREV_VALS + 1);
  // Update prevVals
  for(byte i=0; i<N_PREV_VALS; i++) {
    if (i == N_PREV_VALS - 1) {
      prevVals[i] = positionSmoothed;
    } else {
      prevVals[i] = prevVals[i + 1];
    }
  }
  return positionSmoothed;
}
