/* ======================================================================
 * Use an SPI Device to read a Tesla current shunt. A listener reports.
 */

#include <Arduino.h>
#include <Constants.h>
#include <Parameter.h>
#include <VehicleController.h>
#include <Device.h>
#include <TeslaShunt.h>


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
TeslaShunt devOne(&vc, &current);
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
  Serial.println("Device two reacts to the SPI transaction results of Device one\n");

  while(1) {
    vTaskDelay(100);
  }
}

void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
