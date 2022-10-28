/* ============================== CAN message ============================= 
 * The CanMessage object is identified by its id. It has an optional send
 * interval and holds information in a byte array that can be read and written
 * bit-wise.
 */

#ifndef CanMessage_h
#define CanMessage_h

#include <Arduino.h>
#include <mcp2515.h>
#include "constants.h"


/* ============================== CAN message class =======================
 * It needs an MCP_CAN object to send messages on the bus. For periodic
 * messages, the send interval can be set.
 */
class CanMessage {
  private:
    MCP2515* _canObj;
    uint8_t _frame[LSCF];
    uint32_t _id;
    int _interval;
    long _lastSent;
    bool _checkLSBandLen(int lsb, int len);
    uint8_t _getLastByte(int lsb, int len);
  public:
    void setFrameBytes(uint8_t val=0);
    uint32_t getId();
    void send(int interv=0);
    float readSignalLE(int lsb, int len, float conv=1, int offset=0);
    float readSignalBE(int lsb, int len, float conv=1, int offset=0);
    float readByte(int b, float conv=1, int offset=0);
    void writeSignal(int lsb, int len, long val, float conv=1, int offset=0);
    void writeByte(int b, long val, float conv=1, int offset=0);
    CanMessage();
    CanMessage(uint32_t ident, MCP2515* canObj, int interv);
};

#endif
