/* ============================== Throttle ================================ 
 * This header holds the throttle class that is used to send torque/speed
 * requests (positive and negative) to the controller when the motor is
 * running
 */

#ifndef Throttle_h
#define Throttle_h

#include "Arduino.h"
#include <EEPROM.h>
#include "constants.h"
#include "commUtils.h"
#include "CanManager.h"

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
