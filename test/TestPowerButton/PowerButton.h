/* ============================== Power button ============================
 * Defines a class for the power button object that can start and stop the
 * motor controller and manage a corredponding LED accordingly.
 */

#ifndef PowerButton_h
#define PowerButton_h

#include "Arduino.h"
#include "CanManager.h"
#include "constants.h"

class PowerButton {
  private:
    CanManager* _can;
    byte _pin;
    byte _pinLED;
    bool _MCUready;
    bool _BMSready;
    bool _OBCready;
    bool _keyPosCrank;
    byte _stateLED;
    int _brightnessLED;
    float _prevFadeVal;
    unsigned long _previousMillis;
    unsigned long _lastDebounce;
    void _setLED(byte state);
    void _updateLED();
  public:
    void startMotor();
    void stopMotor();
    void setMCUready(bool state);
    void update();
    void begin(byte pin, byte pinLED, bool MCUready);
    PowerButton(CanManager* can);
};

#endif
