/* ======================================================================
 * Demonstrates an ignition switch and a listening device
 */

#include <Arduino.h>
#include <Parameter.h>
#include <VehicleController.h>
#include <Device.h>
#include <IgnitionSwitch.h>


#define ACC_PIN GPIO_NUM_35
#define ON_PIN GPIO_NUM_32
#define START_PIN GPIO_NUM_33


// Instantiate the VehicleController and the vehicle's Parameters
VehicleController vc;
ParameterInt paramSwitch(0);
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
uint8_t pins[3] = {ACC_PIN, ON_PIN, START_PIN};
int pinModes[3] = {INPUT_PULLDOWN, INPUT_PULLDOWN, INPUT_PULLDOWN};
IgnitionSwitch devOne(&vc, pins, pinModes, &paramSwitch, 200);
DeviceListener devTwo(&vc);


void setup() {
  // Preparations
  Serial.begin(115200); // Start the Serial monitor
  
  Serial.println("===== Starting up the devices =====\n");
  
  devTwo.begin();
  devOne.begin();
  
  Serial.println("===== Starting the test =====");
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
