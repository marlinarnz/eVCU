/* ======================================================================
 * Demonstrates a switch and a listening device
 */

#include <Arduino.h>
#include <Parameter.h>
#include <VehicleController.h>
#include <Device.h>
#include <Switch.h>


#define ISR_PIN GPIO_NUM_32


// Instantiate the VehicleController and the vehicle's Parameters
VehicleController vc;
ParameterBool paramSwitch(0);
ParameterInt paramCount(1);
  

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

    // Start input observation
    // nothing to observe
  };
  
  void shutdown() {
    this->unregisterForValueChanged(0);
  };
  
private:
  void onValueChanged(Parameter* pParamWithNewValue) override {
    if(pParamWithNewValue) {
      switch(pParamWithNewValue->getId()) {
        case 0:
          Serial.println("DeviceListener saw a new value of the switch: "
            +String(paramSwitch.getVal()));
          this->setIntegerValue(&paramCount, paramCount.getVal() + 1);
          break;
        default:
          break;
      }
    }
  };
};


// Instantiate Devices
Switch devOne(&vc, ISR_PIN, INPUT_PULLUP, &paramSwitch, 300);
DeviceListener devTwo(&vc);


void setup() {
  // Preparations
  Serial.begin(9600); // Start the Serial monitor
  
  Serial.println("===== Starting up the devices =====\n");
  
  devTwo.begin();
  devOne.begin();
  
  Serial.println("===== Starting the test =====\n");
  Serial.println("Device two reacts to the input observation of Device one \n"+
                 String("and changes a Parameter, to which Device one reacts.\n"));

  while(1) { // don't leave the scope where the Device instances live
    vTaskDelay(100);
  }
}

void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
