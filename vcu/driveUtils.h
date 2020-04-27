/* ============================== Utility functions ======================= 
 * This header holds all utility funtions for driving including the throttle
 * class
 */

#ifndef driveUtils_h
#define driveUtils_h

#include "Arduino.h"
#include "constants.h"
#include "commUtils.h"

void cool(int motorTemp, int controllerTemp);
void updateRecu();
void updateDirec();

class Throttle {
  private:
    byte _throttlePin;
    byte _brakePedPin;
    CanManager* _can;
    bool _brakePedalFunction;
    float _prevVals[TTSF];
    float _translateThrottleToTorque();
    float _translateBreakPedToTorque();
    float _smoothTorqueReq(float torque);
  public:
    void update();
    void reset();
    void begin(int tPin, int bPin=255);
    Throttle(CanManager* canManager);
};

#endif
