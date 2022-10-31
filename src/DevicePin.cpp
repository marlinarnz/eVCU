#include "DevicePin.h"


/** The constructor attaches the interrupt.
 *  @param pController: VehicleController instance passed to Device
 *  @param pin: GPIO pin for the interrupt
 *  @param debounce: minimum millis between two interrupt yields
 *  @param pinMode: INPUT, INPUT_PULLUP, INPUT_PULLDOWN
 *  @param interruptMode: at pin voltage LOW, CHANGE, RISING, FALLING
 */
DevicePin::DevicePin(VehicleController* pController, uint8_t pin, int debounce, int inputMode, int interruptMode)
  : Device(pController), m_taskHandleOnPinInterrupt(NULL),
    m_pin(pin), m_debounce(debounce)
{
  pinMode(this->m_pin, inputMode);
  attachInterrupt(this->m_pin, std::bind(&DevicePin::isr,this), interruptMode);
}


/** The destructor deletes tasks and detaches the interrupt.
 */
DevicePin::~DevicePin()
{
  detachInterrupt(this->m_pin);
  vTaskDelete(m_taskHandleOnPinInterrupt);
}


/** ISR that invokes the onPinInterruptLoop task.
 *  Uses the ESP32 FunctionalInterrupt to attach class member
 *  functions to Arduino pin interrupts.
 *  FunctionalInterrupt use discussion: https://stackoverflow.com/questions/56389249/how-to-use-a-c-member-function-as-an-interrupt-handler-in-arduino
 *  Using Glue routines: https://arduino.stackexchange.com/questions/89173/attach-the-arduino-isr-function-to-the-class-member
 */
void ARDUINO_ISR_ATTR DevicePin::isr() {
  if(millis() - this->m_lastPinInterrupt > this->m_debounce) {
    this->m_lastPinInterrupt = millis();
    // Notify the object's task
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(this->m_taskHandleOnPinInterrupt,
                           &xHigherPriorityTaskWoken);
    // Trigger a context change
    if(xHigherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
    }
  }
}


/** Logic function to handle hardware inputs from pins.
 *  This function must be filled in the derived class.
 */
void DevicePin::onPinInterrupt()
{}


/** Task function that calls a logic function when notified from ISR.
 *  `onPinInterruptLoop` is designed as a FreeRTOS task function
 *  (i.e. with infinite loop and void pointer input). It runs and consumes
 *  CPU time only if it gets notified for hardware input changes by an
 *  interrupt service routine. Then, it calls the logic function
 *  `onPinInterrupt`.
 */
void DevicePin::onPinInterruptLoop(void* pvParameters)
{
  for(;;) {
    // Wait for a notification from an ISR
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //TODO
    this->onPinInterrupt();
    /*if (DEBUG) {
      PRINT("Debug: onPinInterruptLoop free stack size: "+String(
        uxTaskGetStackHighWaterMark(NULL)))
    }*/
  }
  vTaskDelete(NULL);
}


/** Static adaptor for non-static member function.
 *  FreeRTOS tasks work with static member functions only. This
 *  worksaround has the drawback that tasks must be created by the
 *  corresponding object itself.
 *  @param _this this pointer
 */
void DevicePin::startOnPinInterruptLoop(void* _this)
{
  static_cast<DevicePin*>(_this)->onPinInterruptLoop(NULL);
}


/** Starts tasks for processing value changes and hardware inputs.
 *  Must be called in the derived class's begin() function. FreeRTOS
 *  tasks use stack sizes in words (i.e. stack depth), which
 *  corresponds to 1/4th of the stack size in bytes on 32bit-systems.
 *  @param stackSizeOnValueChanged size of onValueChangedLoop task
 *         stack in bytes. Default is 4096
 *  @param stackSizeOnPinInterrupt size of onPinInterruptLoop task
 *         stack in bytes. Default is 4096
 */
void DevicePin::startTasks(uint16_t stackSizeOnValueChanged,
                           uint16_t stackSizeOnPinInterrupt)
{
  xTaskCreatePinnedToCore(
    this->startOnValueChangedLoop, // function name
    "onValueChanged", // Name for debugging
    (uint16_t)(stackSizeOnValueChanged/4),
    this, // Parameters pointer for the function; must be static
    1, // Priority (1 is lowest)
    &m_taskHandleOnValueChanged, // task handle
    1 // CPU core
  );
  if (m_taskHandleOnValueChanged == NULL) {
    PRINT("Fatal: failed to create task onValueChangedLoop")
  }

  xTaskCreatePinnedToCore(
    this->startOnPinInterruptLoop, // function name
    "onPinInterrupt", // Name for debugging
    (uint16_t)(stackSizeOnPinInterrupt/4),
    this, // Parameters pointer for the function; must be static
    1, // Priority (1 is lowest)
    &m_taskHandleOnPinInterrupt, // task handle
    1 // CPU core
  );
  if (m_taskHandleOnPinInterrupt == NULL) {
    PRINT("Fatal: failed to create task onPinInterruptLoop")
  }
}
