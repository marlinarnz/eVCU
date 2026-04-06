#include "L9026.h"

L9026* L9026::m_instance = nullptr;

#define BASE_ADDR 0x00


/** Singleton instance getter
 *  Will be used by other classes to access driver functionalities.
 *  Instantiates the L9026 object once and initialises the IC.
 *  @param vc VehicleController instance pointer
 *  @param pinIdle pin number of L9026 IDLE pin
 *  @param pinIn0 pin number of L9026 IN0 pin
 *  @param pinIn1 pin number of L9026 IN1 pin
 *  @param pinMOSI pin number of MTDO
 *  @param pinMISO pin number of MTDI
 *  @param pinSCLK pin number of MTCK
 *  @param pinCS pin number of MTMS
 *  @return L9026 object instance pointer, if init was successful
 */
L9026* L9026::getInstance(VehicleController* vc,
  uint8_t pinIdle, uint8_t pinIn0, uint8_t pinIn1,
  uint8_t pinMOSI, uint8_t pinMISO, uint8_t pinSCLK, uint8_t pinCS)
{
  if (!m_instance) {
    // Instantiate
    m_instance = new L9026(vc, pinIdle, pinIn0, pinIn1);
    m_instance->begin(pinMOSI, pinMISO, pinSCLK, pinCS);
  }
  return m_instance;
}


/** Constructor
 *  @param vc VehicleController instance pointer
 *  @param pinIdle pin number of L9026 IDLE pin
 *  @param pinIn0 pin number of L9026 IN0 pin
 *  @param pinIn1 pin number of L9026 IN1 pin
 */
L9026::L9026(VehicleController* vc,
  uint8_t pinIdle, uint8_t pinIn0, uint8_t pinIn1)
  : DeviceSPI(vc), m_pinIdle(pinIdle), m_pinIn0(pinIn0), m_pinIn1(pinIn1),
    m_frameCounter(0), m_channelsHIGH(0b00000011), m_channelsPWMGEN(0),
    m_channelsPWMLED(0), m_channelsBIM(0), m_channelsON(0)
{}


/** Initialise hardware
 *  This function will be called once after object instantiation.
 */
void L9026::initL9026()
{
  pinMode(m_pinIdle, OUTPUT);
  pinMode(m_pinIn0, OUTPUT);
  pinMode(m_pinIn1, OUTPUT);
  
  digitalWrite(m_pinIdle, LOW);
  digitalWrite(m_pinIn0, LOW);
  digitalWrite(m_pinIn1, LOW);
  
  vTaskDelay(pdMS_TO_TICKS(2));
  digitalWrite(m_pinIdle, HIGH); // Idle mode = on
  vTaskDelay(pdMS_TO_TICKS(2));
}


/** Start the IC
 *  Start tasks, init the L9026 and the SPI protocol.
 *  Deletes its own object instance, if SPI init failed.
 *  @param pinMOSI pin number of MTDO
 *  @param pinMISO pin number of MTDI
 *  @param pinSCLK pin number of MTCK
 *  @param pinCS pin number of MTMS
 */
void L9026::begin(uint8_t pinMOSI, uint8_t pinMISO, uint8_t pinSCLK, uint8_t pinCS)
{
  // Start the tasks before sending the first transaction
  this->startTasks(4096, 8192);
  
  // Init the IC pins
  this->initL9026();
  
  // Init SPI communication
  configSPI_t config;
  config.pinMOSI=pinMOSI;
  config.pinMISO=pinMISO;
  config.pinSCLK=pinSCLK;
  config.pinCS1=pinCS;
  config.speed_hz=1000000;
  config.dutyCycle=128;
  bool success = this->initSerialProtocol(config);
  
  if (success) {
    // Configure PWM generators and put device in active mode
    this->sendFrame(BASE_ADDR + 0x02, 0b00000010);
    this->sendFrame(BASE_ADDR + 0x03, 0b00000010);
    this->setPWM(0, true, true);
    this->sendFrame(BASE_ADDR + 0x02, 0b01000010);
  }
  else {
    delete m_instance;
  }
}
void L9026::begin()
{
  // Start the tasks before sending the first transaction
  this->startTasks(4096, 8192);
  // Init the IC pins
  this->initL9026();
}


/** End the Serial protocol
 */
void L9026::shutdown()
{
  this->endSerialProtocol();
  //delete m_instance;
}


/** No reaction on SPI incoming frames
 *  Could possibly execute validity checks at the responds to the
 *  previous frame sent.
 *  @param recvBuf data buffer with message from the slave device
 *  @param len length of the receive data buffer
 *  @param transId transaction ID, if set in call to `setTransactionPeriodic()`
 */
//void L9026::onSerialEvent(void* recvBuf, uint8_t len, uint8_t transId){}


/** This driver is not interested in other Devices.
 */
//void L9026::onValueChanged(Parameter* pParam){}


/** Switch output channel
 *  Use the SPI protocol to switch the given channel on or off.
 *  Frame name: PWM_SPI
 *  @param channel [0-7] channel number
 *  @param onOff boolean desired channel state
 *  @return boolean if the transaction was successful or not
 */
bool L9026::switchOutputChannel(uint8_t channel, bool onOff)
{
  if (channel > 7) {return false;}
  
  if (onOff) {m_channelsON |= (1 << channel);}
  else {m_channelsON &= ~(1 << channel);}
  
  return this->sendFrame(BASE_ADDR + 0x06, m_channelsON);
}


/** Set the frequency of a PWM channel
 *  Use the SPI protocol to adjust the PWM frequency of the desired channel.
 *  Frame name: PWM_GEN_DC and PWM_LED_DC
 *  @param frequency integer frequency in percent
 *  @param pwmGEN boolean whether to adjust GEN channel
 *  @param pwmLED boolean whether to adjust LED channel
 *  @return boolean if the transaction was successful or not
 */
bool L9026::setPWM(uint8_t frequency, bool pwmGEN, bool pwmLED)
{
  bool success = false;
  uint8_t val = uint8_t(frequency * 2.55);
  if (pwmGEN) {
    success &= this->sendFrame(BASE_ADDR + 0x0B, val);
  }
  if (pwmLED) {
    success &= this->sendFrame(BASE_ADDR + 0x0C, val);
  }
  
  return success;
}


/** Configure output channel for PWM
 *  Use the SPI protocol to attach the channel to either the GEN or
 *  LED PWM channel. `pwmGEN` and `pwmLED` cannot both be `true`.
 *  Both channels are initiated with 490.2Hz.
 *  Frame name: MAP_PWM and PWM_SEL
 *  @param channel [0-7] channel number
 *  @param pwmGEN boolean whether to attach channel to GEN
 *  @param pwmLED boolean whether to attach channel to LED
 *  @return boolean if the transaction was successful or not
 */
bool L9026::configurePWM(uint8_t channel, bool pwmGEN, bool pwmLED)
{
  if (channel > 7) {return false;}
  if ((pwmGEN) && (pwmLED)) {return false;}
  
  if (pwmGEN) {m_channelsPWMGEN |= (1 << channel);}
  else {m_channelsPWMGEN &= ~(1 << channel);}
  if (pwmLED) {m_channelsPWMLED |= (1 << channel);}
  else {m_channelsPWMLED &= ~(1 << channel);}
  
  bool success = false;
  success &= this->sendFrame(BASE_ADDR + 0x09, m_channelsPWMGEN | m_channelsPWMLED);
  success &= this->sendFrame(BASE_ADDR + 0x0A, m_channelsPWMLED);
  
  return success;
}


/** Configure output channel high or low
 *  Use the SPI protocol to set the channel as low-side or high-
 *  side switch. Channels 0 and 1 can only be high-side.
 *  Frame name: CFG_0
 *  @param channel [0-7] channel number
 *  @param highSwitch boolean true for high, false for low
 *  @return boolean if the transaction was successful or not
 */
bool L9026::configureHSLS(uint8_t channel, bool highSwitch)
{
  if (channel > 7) {return false;}
  if ((channel < 2) && (!highSwitch)) {return false;}
  
  if (highSwitch) {m_channelsHIGH |= (1 << channel);}
  else {m_channelsHIGH &= ~(1 << channel);}
  
  return this->sendFrame(BASE_ADDR + 0x01, m_channelsHIGH);
}


/** Configure output channel bulb inrush mode
 *  Use the SPI protocol to switch bulb inrush mode on or off.
 *  Frame name: BIM
 *  @param channel [0-7] channel number
 *  @param onOff boolean true for on, false for off
 *  @return boolean if the transaction was successful or not
 */
bool L9026::configureBIM(uint8_t channel, bool onOff)
{
  if (channel > 7) {return false;}
  
  if (onOff) {m_channelsBIM |= (1 << channel);}
  else {m_channelsBIM &= ~(1 << channel);}
  
  return this->sendFrame(BASE_ADDR + 0x04, m_channelsBIM);
}


/** Send the frame
 *  Package the data into a frame and send it out.
 *  @param addr 5 bit address
 *  @param frame 8 bit frame with data
 *  @return true (frame validation not implemented)
 */
bool L9026::sendFrame(uint8_t address, uint8_t data)
{
  uint16_t frame = buildFrame(true, address, data);
  
  uint8_t buffer[2];
  buffer[0] = frame >> 8;
  buffer[1] = frame & 0xFF;

  this->setTransactionPeriodic(0, buffer, 2, address);
  return true;
}


/** Build SPI frame
 *  Based on the MOSI frame structure documented in the datasheet:
 *  - 1 command bit R/W, ‘1’ for write, ‘0’ for read
 *  - 5 ADDRESS bits
 *  - 8 DATA bits
 *  - 1 PARITY bit, odd parity, calculated on bit [2:15]
 *  - 1 FR_CNT frame counter bit
 *  @param write boolean whether it is a write (true) or read frame
 *  @param addr address bits contained in 5 last bits
 *  @param data 8 bit frame to transmit
 *  @return 16 bit frame ready for transaction
 */
uint16_t L9026::buildFrame(bool write, uint8_t addr, uint8_t data)
{
  uint16_t frame = 0;
  frame |= (write ? 1 : 0) << 15;
  frame |= (addr & 0x1F) << 10;
  frame |= (data << 2);
  frame |= (m_frameCounter & 0x01);
  uint8_t parity = calcParity(frame);
  frame |= parity << 1;
  m_frameCounter ^= 1;

  return frame;
}


/** Calculate parity
 *  1 bit, odd parity, calculated on bit [2:15]
 *  @param frame 16 bit frame to check parity at
 *  @return 1 bit odd parity
 */
uint8_t L9026::calcParity(uint16_t frame)
{
  uint8_t parity = 0;
  for (int i = 2; i < 16; i++) {
    parity ^= (frame >> i) & 0x01;
  }
  return !parity;
}

