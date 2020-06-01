

// This is a mock

#include "CanManager.h"

extern bool on;
extern bool returnCheckError;
extern float returnReadSignal;

CanManager::CanManager() {
  ;
}

float CanManager::readSignal(long id, int lsb, int len, float conv, int offset) {
  float val = returnReadSignal;
  return val;
}

void CanManager::writeSignal(long id, int lsb, int len, int val, float conv, int offset) {
  if(id==VCU1 && lsb==VCU1_KeyPosition_LSB && len==VCU1_KeyPosition_LEN
     && val==VCU1_KeyPosition_CRANK) {
    on = true;
  }
  else if (id==VCU1 && lsb==VCU1_KeyPosition_LSB && len==VCU1_KeyPosition_LEN
           && val==VCU1_KeyPosition_ACC) {
    on = false;
  }
}

bool CanManager::checkError() {
  return returnCheckError;
}
