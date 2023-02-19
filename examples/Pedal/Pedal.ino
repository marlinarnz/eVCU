/* ======================================================================
 * Demonstrates a pedal and a listening device
 */

#include <Arduino.h>
#include <Parameter.h>
#include <VehicleController.h>
#include <Device.h>
#include <Pedal.h>


#define PEDAL_PIN GPIO_NUM_32


// Instantiate the VehicleController and the vehicle's Parameters
VehicleController vc;
ParameterDouble position(0);
  

// Define the Device child class
class DeviceListener : public Device
{
public:
  DeviceListener(VehicleController* vc) : Device(vc) {};
  void begin() {
    this->startTasks(8000);
    this->registerForValueChanged(0);
  };
  
  void shutdown() {
    this->unregisterForValueChanged(0);
  };
  
private:
  void onValueChanged(Parameter* pParamWithNewValue) override {
    if(pParamWithNewValue) {
      switch(pParamWithNewValue->getId()) {
        case 0:
          Serial.println("Pedal at new position: "
            +String(position.getVal()));
          break;
        default:
          break;
      }
    }
  };
};


// Instantiate Devices
Pedal devOne(&vc, PEDAL_PIN, 10, &position);
DeviceListener devTwo(&vc);


void setup() {
  // Preparations
  Serial.begin(9600); // Start the Serial monitor
  
  Serial.println("===== Starting up the devices =====\n");
  
  devTwo.begin();
  devOne.begin();
  
  Serial.println("===== Starting the test =====\n");
  Serial.println("Device two reacts to the input observation of Device one.");

  while(1) { // don't leave the scope where the Device instances live
    vTaskDelay(100);
  }
}

void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
