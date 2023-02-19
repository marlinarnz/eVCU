/* ======================================================================
 * Set up a CAN bus and listen to other nodes.
 */

#include <Arduino.h>
#include <VehicleController.h>
#include <DeviceCAN.h>


// Instantiate the VehicleController and the vehicle's Parameters
VehicleController vc;


// Define CAN Manager
class CANManager : public DeviceCAN
{
public:
  CANManager(VehicleController* vc) : DeviceCAN(vc) {};

  void begin() {
    this->startTasks(8000, 8000);

    configCAN_t config;
    config.mode = TWAI_MODE_NORMAL;
    config.txPin = GPIO_NUM_17;
    config.rxPin = GPIO_NUM_16;
    config.speed = 250000;
    this->initSerialProtocol(config);

    // Set a periodic message
    /*twai_message_t* msg = new twai_message_t;
    msg->identifier = 0x1806E5F0;
    msg->data_length_code = 1;
    msg->rtr = 0;
    msg->self = 0;
    msg->ss = 0;
    msg->extd = 1;
    msg->data[0] = 1;
    this->setTransactionPeriodic(msg, 1000);*/
  };

  void shutdown() {
    this->endSerialProtocol();
  };

private:
  void onMsgRcv(twai_message_t* pMsg) override {
    Serial.print("0x" + String(pMsg->identifier, HEX) + "  ");
    for (uint8_t i=0; i<pMsg->data_length_code; i++) {
      Serial.print(String(pMsg->data[i], HEX) + " ");
    }
    Serial.println("");
  };

  void onRemoteFrameRcv(twai_message_t* pMsg) override {
    Serial.println("Received remote frame with ID 0x" + String(pMsg->identifier, HEX));
  };
};

CANManager listener(&vc);



void setup() {
  // Preparations
  Serial.begin(9600);
  listener.begin();
  while(1) {
    vTaskDelay(100);
  }
}


void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
