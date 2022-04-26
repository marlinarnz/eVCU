#include "TeslaShunt.h"


/** The constructor saves the current Parameter as member.
 *  @param vc pointer to the VehicleController instance
 *  @param pParam pointer to the ParameterInt instance that
 *                shall inform other Devices about the current
 */
TeslaShunt::TeslaShunt(VehicleController* vc, ParameterInt* pParam)
  : DeviceSPI(vc), m_pParam(pParam)
{}


/** Start the Serial protocol and send the first transaction.
 */
void TeslaShunt::begin()
{
  // Start the protocol
  configSPI_t config;
  config.speed_hz = 500000;
  this->initSerialProtocol(config);

  // Start the tasks before sending the first transaction
  this->startTasks(4096, 8192);

  // Send the first message
  byte len = 11;
  byte dataBuf[] = {0x7f, 0x7f, 0x21, 0xde, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00};
  this->setTransactionPeriodic(0, dataBuf, len, 1);
}


/** End the Serial protocol.
 */
void TeslaShunt::shutdown()
{
  this->endSerialProtocol();
}


/** Handles transaction results with the shunt.
 *  Sends transactions that prepare the shunt to send the current and then
 *  loops the transaction that yields the current.
 *  @param recvBuf data buffer with message from the slave device
 *  @param len length of the receive data buffer
 *  @param transId transaction ID, if set in call to `setTransactionPeriodic()`
 */
void TeslaShunt::onSerialEvent(void* recvBuf, uint8_t len, uint8_t transId)
{
  byte* dataBuf = (byte*)recvBuf;
  /*for (int I = 0; I < len; I++) {
    Serial.print(*(dataBuf+I), HEX);
    Serial.print(" ");
  }
  Serial.println();*/
  byte sendBuf[8] = {0};
  switch(transId) {
    case 1:
      sendBuf[0] = 0x4f;
      sendBuf[7] = 0xBC;
      this->setTransactionPeriodic(0, sendBuf, 8, 2);
      break;
    case 2:
      sendBuf[0] = 0x4E;
      sendBuf[7] = 0x63;
      this->setTransactionPeriodic(0, sendBuf, 8, 3);
      break;
    case 3:
      sendBuf[0] = 0x4E;
      sendBuf[7] = 0x63;
      this->setTransactionPeriodic(0, sendBuf, 8, 4);
      break;
    case 4:
      sendBuf[0] = 0x16;
      sendBuf[3] = 0x02;
      sendBuf[7] = 0x33;
      this->setTransactionPeriodic(0, sendBuf, 8, 5);
      break;
    case 5:
      sendBuf[0] = 0x14;
      sendBuf[3] = 0xDC;
      sendBuf[4] = 0x05;
      sendBuf[5] = 0xC2;
      sendBuf[6] = 0x01;
      sendBuf[7] = 0x83;
      this->setTransactionPeriodic(0, sendBuf, 8, 6);
      break;
    case 6:
      sendBuf[0] = 0x13;
      sendBuf[3] = 0x04;
      sendBuf[4] = 0x01;
      sendBuf[5] = 0x02;
      sendBuf[6] = 0x08;
      sendBuf[7] = 0x94;
      this->setTransactionPeriodic(0, sendBuf, 8, 7);
      break;
    case 7:
      sendBuf[0] = 0x17;
      sendBuf[7] = 0xC0;
      this->setTransactionPeriodic(0, sendBuf, 8, 8);
      break;
    case 8:
      // Send the current buffer periodically
      sendBuf[0] = 0x41;
      sendBuf[7] = 0x9A;
      this->setTransactionPeriodic(1000, sendBuf, 8, 9);
      break;
    case 9:
      // Communicate the current
      {
        int curr = ((int32_t( *(dataBuf+6)*256 + *(dataBuf+5)
                             )*256 + *(dataBuf+4))*256 + *(dataBuf+3));
        this->setIntegerValue(m_pParam, curr);
      }
      break;
    default:
      PRINT("Info: received SPI buffer with unknown ID: "+String(transId))
      break;
  }
}
