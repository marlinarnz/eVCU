#ifndef CanManager_h
#define CanManager_h

#include "Arduino.h"
#include "constants.h"

// This is a mock

class CanManager {
  public:
    float readSignal(long id, int lsb, int len, float conv=1, int offset=0);
    void writeSignal(long id, int lsb, int len, int val, float conv=1, int offset=0);
    bool checkError();
    bool checkMCUready(bool quick=false);
    bool checkBMSready(bool quick=false);
    CanManager();
};

#endif
