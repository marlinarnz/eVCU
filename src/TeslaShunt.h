#ifndef TESLASHUNT_H
#define TESLASHUNT_H

#include "DeviceSPI.h"


/** Reads a Tesla Current Shunt.
 *  Serial communication and wiring from Tom Debree
 *  https://github.com/Tom-evnut/Tesla-Current-Shunt
 */
class TeslaShunt : public DeviceSPI
{
public:
  TeslaShunt(VehicleController* vc, ParameterInt* pParam);
  void begin();
  void shutdown();

private:
  void onSerialEvent(void* recvBuf, uint8_t len, uint8_t transId);
  ParameterInt* m_pParam;
};

#endif
