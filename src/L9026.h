#ifndef L9026_H
#define L9026_H

#include "DeviceSPI.h"
#include <Arduino.h>

/** L9026 SPI driver class
 *  This class implements an SPI driver for the ST L9026 8-channel
 *  high/low-side driver IC. The class is implemented as a singleton
 *  because only one IC instance is expected in the system. The
 *  `L9026Channel` class implements control of one L9026 output channel,
 *  using this SPI and pin protocol.
 *  SPI protocol documentation in section 11:
 *  https://www.st.com/resource/en/datasheet/l9026.pdf
 *  Start-up sequence documentation in STEVAL manual, p. 8:
 *  https://www.st.com/resource/en/user_manual/um3339-stevall9026-evaluation-board-user-manual-stmicroelectronics.pdf
 */
class L9026 : public DeviceSPI
{
public:
  static L9026* getInstance(VehicleController* vc,
    uint8_t pinIdle, uint8_t pinIn0, uint8_t pinIn1,
    uint8_t pinMOSI, uint8_t pinMISO, uint8_t pinSCLK, uint8_t pinCS);
  bool switchOutputChannel(uint8_t channel, bool onOff);
  bool setPWM(uint8_t frequency, bool pwmGEN, bool pwmLED);
  bool configurePWM(uint8_t channel, bool pwmGEN, bool pwmLED);
  bool configureHSLS(uint8_t channel, bool highSwitch);
  bool configureBIM(uint8_t channel, bool onOff);
  void begin();
  void shutdown();

private:
  L9026(VehicleController* vc, uint8_t pinIdle, uint8_t pinIn0, uint8_t pinIn1);
  static L9026* m_instance;
  //void onSerialEvent(void* recvBuf, uint8_t len, uint8_t transId);
  //void onValueChanged(Parameter* pParam);
  bool sendFrame(uint8_t address, uint8_t data);
  uint16_t buildFrame(bool write, uint8_t addr, uint8_t data);
  uint8_t calcParity(uint16_t frame);
  void initL9026();
  uint8_t m_pinIdle;
  uint8_t m_pinIn0;
  uint8_t m_pinIn1;
  uint8_t m_frameCounter;
  uint8_t m_channelsHIGH;
  uint8_t m_channelsPWMGEN;
  uint8_t m_channelsPWMLED;
  uint8_t m_channelsBIM;
  uint8_t m_channelsON;
};

#endif
