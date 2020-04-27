/* ============================== Utility functions =======================
 * This header holds all utility funtions for communication including the
 * CanManager class
 */

#ifndef commUtils_h
#define commUtils_h

#include <Arduino.h>
#include <mcp_can.h>
#include "constants.h"
#include "CanMessage.h"


/* ============================== Utility functions =======================
 *  Non-object oriented functions to spread and receive all kind of news
 */
void report(String msg, int level, bool warn=false); //TODO
void setWarningLevel(int lvl);
void resetEEPROM();


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
