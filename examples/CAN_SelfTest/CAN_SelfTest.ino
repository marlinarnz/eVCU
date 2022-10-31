/* ======================================================================
 * Use the ESP32 TWAI driver to set up a CAN bus self test.
 * The ESP32 example:
 * https://github.com/espressif/esp-idf/blob/667edfa7a9d64ac8e73f9c8b48d112dd82797370/examples/peripherals/twai/twai_self_test/main/twai_self_test_example_main.c
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html
 */

#include <Arduino.h>
#include <Constants.h>
#include <Parameter.h>
#include <VehicleController.h>
#include <Device.h>
#include <DeviceCAN.h>


// Instantiate the VehicleController and the vehicle's Parameters
VehicleController vc;
ParameterInt current(0);


// Define CAN Manager
class CANManager : public DeviceCAN
{
public:
  CANManager(VehicleController* vc) : DeviceCAN(vc), msg(NULL) {
    // Define message
    msg = new twai_message_t;
    msg->data_length_code = 1;
    msg->identifier = 0x1;
    msg->self = 1; // self reception request
    msg->data[0] = 0;
  };
  void begin() {
    Serial.println("Started CAN Manager");
    this->startTasks(8000, 8000);
    configCAN_t config;
    config.mode = TWAI_MODE_NO_ACK;
    this->initSerialProtocol(config);
    
    // Define message(s) to send
    this->setTransactionPeriodic(msg, 1000);
  };

  void shutdown() {
    this->endSerialProtocol();
  };

private:
  twai_message_t* msg;
  void onMsgRcv(twai_message_t* pMsg) override {
    int val = pMsg->data[0];
    this->setIntegerValue(&current, val+1);
  };
};


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
CANManager devTwo(&vc);



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


/*

Error receiving a CAN message: CAN driver is not installed
Error receiving a CAN message
Error receiving a CAN message: CAN driver is not installed
Error receiving a CAN message
...
Error sending CAN message: Try to start the CAN driver
Error sending CAN message because in listen only mode
Error sending CAN message because of wrong parameters or sedn queue disabled
Error receiving a CAN message: CAN driver is not installed
Error receiving a CAN message
Error sending CAN message: Try ...
*/

void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
