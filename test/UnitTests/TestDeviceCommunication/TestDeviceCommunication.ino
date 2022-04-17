/* ======================================================================
 * These tests emulate input pin/serial value changes through serial
 * monitor input.
 * 
 * Useful links for ESP32 Rx interrupts:
 * - CAN bus (TWAI):
 *  - https://github.com/espressif/esp-idf/blob/667edfa7a9d64ac8e73f9c8b48d112dd82797370/examples/peripherals/twai/twai_self_test/main/twai_self_test_example_main.c
 *  - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html#_CPPv416twai_read_alertsP8uint32_t10TickType_t
 * - UART (with events):
 *  - https://github.com/espressif/esp-idf/blob/667edfa7a9d64ac8e73f9c8b48d112dd82797370/examples/peripherals/uart/uart_events/main/uart_events_example_main.c
 *  - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/uart.html#uart-api-using-interrupts
 * - SPI (as master):
 *  - https://github.com/espressif/esp-idf/tree/667edfa7a9d64ac8e73f9c8b48d112dd82797370/examples/peripherals/spi_slave
 *  - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html#interrupt-transactions
 * 
 * Useful concepts to allow ISRs as member functions:
 * https://forum.arduino.cc/t/class-member-function-as-an-interrupt-service-routine/104337
 */

#include <Arduino.h>
#include "Constants.h"
#include "Parameter.h"
#include "VehicleController.h"
#include "Device.h"
#include "DevicePin.h"


#define ISR_PIN 15


// Instantiate the VehicleController and the vehicle's Parameters
VehicleController vc;
ParameterBool paramSwitch(0);
ParameterInt paramCount(1);
  

// Define the Device child classes
class Switch : public DevicePin
{
public:
  Switch(VehicleController* vc) : DevicePin(vc) {};
  
  void begin() override {
    Serial.println("Starting Switch");
    this->startTasks(8000,8000);
    
    // Register Device for other Parameters
    this->registerForValueChanged(1);

    // Start input pins
    pinMode(ISR_PIN, INPUT_PULLUP);
  };
  
  void shutdown() {
    detachInterrupt(ISR_PIN);
    this->unregisterForValueChanged(1);
  };
  
private:
  void onValueChanged(Parameter* pParamWithNewValue) override {
    if(pParamWithNewValue) {
      switch(pParamWithNewValue->getId()) {
        case 1:
          Serial.println("Switch saw a value change in Parameter 1: "+String(
            paramCount.getVal()));
          break;
        default:
          break;
      }
    }
  };

  void onPinInterrupt() override {
    // Notify other devices of the parameter change
    this->setBooleanValue(&paramSwitch, digitalRead(ISR_PIN));
  };
};


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
Switch devOne(&vc);
DeviceListener devTwo(&vc);




// Here comes the ISR (after the objects are initialised)
volatile long lastPinInterrupt = 0;

void IRAM_ATTR pinInterrupt() {
  if(millis() - lastPinInterrupt > 400) {
    lastPinInterrupt = millis();
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(devOne.getTaskHandle(),
                           &xHigherPriorityTaskWoken);
    // Trigger a context change
    if(xHigherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
    }
  }
}




void setup() {
  // Preparations
  Serial.begin(9600); // Start the Serial monitor
  Serial.println("===== Starting the initialisation =====\n");
  
  vc.registerParameter(&paramSwitch); // Register Parameters
  vc.registerParameter(&paramCount);
  
  Serial.println("===== Starting up the devices =====\n");
  
  devTwo.begin();
  devOne.begin();
  
  attachInterrupt(ISR_PIN, pinInterrupt, CHANGE); // Now we can attach an interrupt
  //gpio_isr_handler_add((gpio_num_t)ISR_PIN, (gpio_isr_t)pinInterrupt, (void*)NULL);
  
  Serial.println("===== Starting the tests =====\n");
  // Test 1
  Serial.println("Test 1: Device two reacts to the input observation of Device \n"+
                 String("one and changes a Parameter, to which Device one reacts.\n"));

  while(1) { // don't leave the scope where the Device instances live
    vTaskDelay(100);
  }
}

void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
