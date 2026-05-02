/* ======================================================================
 * Demonstrates a pedal and a listening device
 */

#include <Arduino.h>
#include <Parameter.h>
#include <VehicleController.h>
#include <Device.h>
#include <Pedal.h>
#include <SecuredLinkedListMap.h>


#define PEDAL_PIN 4
#define PEDAL_VMAX 3.5
#define V_DIVIDER_RATIO 1.0


// Instantiate the VehicleController and the vehicle's Parameters
VehicleController vc;
ParameterDouble position(0);
SecuredLinkedListMap<double, double> pedalMap;
  

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
Pedal devOne(&vc, PEDAL_PIN, 10, &position, &pedalMap, PEDAL_VMAX, V_DIVIDER_RATIO);
DeviceListener devTwo(&vc);


void setup() {
  // Preparations
  Serial.begin(115200); // Start the Serial monitor
  
  Serial.println("===== Starting up the devices =====\n");

  pedalMap.put(0.0, 0.0);
  pedalMap.put(PEDAL_VMAX * 0.1, 0.0); // Start point
  pedalMap.put(PEDAL_VMAX, 100.0); // Maximum point
  
  devTwo.begin();
  devOne.begin();
  
  Serial.println("===== Starting the test =====\n");
  Serial.println("Device two reacts to the input observation of Device one.");

  while(1) { // don't leave the scope where the Device instances live
    vTaskDelay(1000);
    Serial.println("Analog measurement: " + String(analogRead(PEDAL_PIN)));
  }
}

void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
