/* ======================================================================
 * Use the ESP32 TWAI driver to set up a CAN bus self test.
 * The ESP32 example:
 * https://github.com/espressif/esp-idf/blob/667edfa7a9d64ac8e73f9c8b48d112dd82797370/examples/peripherals/twai/twai_self_test/main/twai_self_test_example_main.c
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html
 */

#include <Arduino.h>
#include "Constants.h"
#include "Parameter.h"
#include "VehicleController.h"
#include "Device.h"


// Instantiate the VehicleController and the vehicle's Parameters
VehicleController vc;
ParameterInt current(0);
  

// Define the Device child class
class DeviceListener : public Device
{
public:
  DeviceListener(VehicleController* vc) : Device(vc) {};
  void begin() {
    Serial.println("Started DeviceListener");
    this->startTasks(8000);
    
    // Register Device for other Parameters
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
          Serial.println("Current received: "+String(current.getVal()));
          break;
        default:
          break;
      }
    }
  };
};


// Instantiate Devices
DeviceListener devOne(&vc);
DeviceListener devTwo(&vc);



void setup() {
  // Preparations
  Serial.begin(9600); // Start the Serial monitor
  Serial.println("===== Starting the initialisation =====\n");
  
  vc.registerParameter(&current); // Register Parameters
  
  Serial.println("===== Starting up the devices =====\n");
  
  devTwo.begin();
  devOne.begin();
  
  Serial.println("\n===== Starting the test =====");
  Serial.println("Self testing CAN bus communication\n");

  while(1) {
    vTaskDelay(100);
  }
}

void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
