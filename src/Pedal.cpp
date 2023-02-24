#include "Pedal.h"


#define ADC_WIDTH 9
#define ADC_RESOLUTION 511


/** The constructor configures the analog pin.
 *  @param vc pointer to the VehicleController instance
 *  @param pin analog GPIO pin hooked up to the pedal
 *  @param readInterval time in ms between two position reads
 *  @param pParam pointer to the ParameterDouble instance that
 *                informs other Devices about pedal position in %
 */
Pedal::Pedal(VehicleController* vc, uint8_t pin, int readInterval, ParameterDouble* pParam)
  : DeviceLoop(vc, readInterval),
    m_pParam(pParam), m_pin(pin), m_prevVals{}
{
  // Set all values in smoothening array to 0
  for (int i=0; i<N_PREV_VALS; i++) {
    m_prevVals[i] = 0;
  }
  // Good guidance through ADC on ESP32 and Arduino:
  // https://deepbluembedded.com/esp32-adc-tutorial-read-analog-voltage-arduino/
  // Attach the pin to ADC and clear previous settings
  //adcAttachPin(pin);
  // Set the pin's suggested voltage range (11 ~ full range 0-3.3V)
  analogSetPinAttenuation(pin, ADC_11db);
  // Set the ADC resolution in bits (9=511 to 12=4095)
  analogReadResolution(ADC_WIDTH);
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
  // Start tasks
  this->startTasks(4096, 8192);
}


/** At shutdown, nothing happens.
 */
void Pedal::shutdown()
{}


/** Change the pedal position accordingly.
 *  Sets a new percentage value in the Parameter.
 */
void Pedal::onLoop()
{
  // Calculate the value
  double position = (analogRead(m_pin) * 99.7 / ADC_RESOLUTION);
  // Set the value
  this->setDoubleValue(m_pParam, smoothen(position));
}


/** Smoothens the pedal position.
 *  Takes the floating average of the n previous measurements where n
 *  is defined by constant `N_PREV_VALS`.
 *  @param newPosition: value to be smoothened
 *  @return float smoothened value
 */
float Pedal::smoothen(float newPosition)
{
  /* The smoothening via array also delays the pedal
  // Compute the floating average
  float sum = 0;
  for(byte i=0; i<N_PREV_VALS; i++) {
    sum += m_prevVals[i];
  }
  float positionSmoothed = (newPosition + sum) / (N_PREV_VALS + 1);
  // Update prevVals
  for(byte i=0; i<N_PREV_VALS; i++) {
    if (i == N_PREV_VALS - 1) {
      m_prevVals[i] = positionSmoothed;
    } else {
      m_prevVals[i] = m_prevVals[i + 1];
    }
  }
  return positionSmoothed;
  */

  // Round to one decimal behind the comma
  return (int)(newPosition * 10) * 0.1;
}
