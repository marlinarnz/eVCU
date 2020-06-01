

// This is a mock

#include "CanManager.h"

extern float motorRpm;

extern int motorMode;
extern int brakePedStatus;
extern int torqueRequest;
extern int directionRequest;

CanManager::CanManager() {
  ;
}

float CanManager::readSignal(long id, int lsb, int len, float conv, int offset) {
  // Return motor rpm
  return motorRpm;
}

void CanManager::writeSignal(long id, int lsb, int len, int val, float conv, int offset) {
  if(id==VCU1 && lsb==VCU1_GearLeverPosSts_LSB && len==VCU1_GearLeverPosSts_LEN) {
    directionRequest = val;
  }
  else if (id==VCU1 && lsb==VCU1_MotorMode_LSB && len==VCU1_MotorMode_LEN) {
    motorMode = val;
  }
  else if (id==VCU1 && lsb==VCU1_BrakePedalSts_LSB && len==VCU1_BrakePedalSts_LEN) {
    brakePedStatus = val;
  }
  else if (id==VCU1 && lsb==VCU1_TorqueReq_LSB && len==VCU1_TorqueReq_LEN) {
    torqueRequest = val;
  }
}

bool CanManager::checkError() {
  return true;
}
