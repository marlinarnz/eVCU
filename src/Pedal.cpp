#include "Pedal.h"


#define ADC_WIDTH 9
#define ADC_RESOLUTION 511


/** The constructor configures the analog pin.
 *  @param vc pointer to the VehicleController instance
 *  @param pin analog GPIO pin hooked up to the pedal
 *  @param readInterval time in ms between two position reads
 *  @param pParam pointer to the ParameterDouble instance that
 *                informs other Devices about pedal position in %
 *  @param map dictionary to translate voltage readings into %
 *  @param vref ADC reference voltage
 *  @param dividerRatio voltage divider ratio
 *  @param pParamInhibit pointer to the ParameterBool instance
 *                       that overwrites the pedal position with 0
 */
Pedal::Pedal(VehicleController* vc, uint8_t pin, int readInterval, ParameterDouble* pParam, const SecuredLinkedListMap<double, double>& map, double vref, double dividerRatio, ParameterBool* pParamInhibit)
  : DeviceLoop(vc, readInterval),
    m_pParam(pParam), m_pParamInhibit(pParamInhibit), m_pin(pin),
    m_map(map), m_vref(vref), m_dividerRatio(dividerRatio), m_prevVals{}
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

  // Set pin mode
  //pinMode(pin, INPUT);
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
  if (m_pParamInhibit) {
    if (m_pParamInhibit->getVal()) {
      this->setDoubleValue(m_pParam, 0.0);
      return;
    }
  }
  // Set the value
  this->setDoubleValue(m_pParam, smoothen(mapADC(analogRead(m_pin))));
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


/** Maps ADC value to percentage.
 *  Converts ADC reading to voltage and performs linear interpolation
 *  between neighbouring voltage-percentage pairs stored in m_map.
 *  @param adc ADC reading
 *  @return percentage value
 */
double Pedal::mapADC(int adc)
{
  int size = m_map.size();
  if (size == 0) {return 0.0;}

  // Convert ADC to voltage
  double voltage = ((double)adc / ADC_RESOLUTION) * m_vref * m_dividerRatio;

  // Copy map elements
  SecuredLinkedListMapElement<double, double>* elements =
    new SecuredLinkedListMapElement<double, double>[size];
  m_map.getAll(elements);

  double prevVoltage = elements[0].key;
  double prevPercent = elements[0].value;

  for (int i = 1; i < size; i++) {
    double currVoltage = elements[i].key;
    double currPercent = elements[i].value;

    if (voltage <= currVoltage) {
      double t = (voltage - prevVoltage) / (double)(currVoltage - prevVoltage);
      delete[] elements;
      return prevPercent + t * (currPercent - prevPercent);
    }

    prevVoltage = currVoltage;
    prevPercent = currPercent;
  }

  // default to the last element in the map
  double result = prevPercent;
  delete[] elements;
  return result;
}
