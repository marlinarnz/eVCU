/* ============================== CAN Manager ============================= 
 * The CanManager class handles all CAN bus communication and saves messages
 * from other ECUs.
 */

#ifndef CanManager_h
#define CanManager_h

#include <Arduino.h>
#include <mcp_can.h>
#include "constants.h"
#include "CanMessage.h"
#include "commUtils.h"
#include <EEPROM.h>


/* ============================== CAN manager class =======================
 * Class which holds the last update for every known CAN signal. Uses a
 * MCP_CAN class v0.1 (written by SeeedStudio on 2012-4-24)
 */
class CanManager {
  private:
    MCP_CAN* _canObj;
  public:
    void update();
    void begin();
    void sendMessage(uint32_t id, int interval=0);
    int checkError();
    float readSignal(uint32_t id, int lsb, int len, float conv=1, int offset=0);
    void writeSignal(uint32_t id, int lsb, int len, long val, float conv=1, int offset=0);
    CanMessage messagesVCU[N_VCU_MESSAGES];
    CanMessage messagesOther[N_OTHER_MESSAGES];
    CanManager();
    CanManager(MCP_CAN* canObj);
};

#endif
