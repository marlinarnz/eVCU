#include "DevicePin.h"


/** The constructor inits the additional task handle and calls Device().
 */
DevicePin::DevicePin(VehicleController* pController)
  : Device(pController), m_taskHandleOnPinInterrupt(NULL)
{}


/** The destructor deletes tasks.
 */
DevicePin::~DevicePin()
{
  vTaskDelete(m_taskHandleOnPinInterrupt);
}


/** Get the handle for the onPinInterruptLoop task.
 *  Useful for equipping global ISR functions, if they are defined
 *  after the DevicePin object is instantiated.
 *  @return handle for the onPinInterruptLoop task
 */
TaskHandle_t DevicePin::getTaskHandle()
{
  return m_taskHandleOnPinInterrupt;
}


/** Logic function to handle hardware inputs from pins.
 *  This function must be filled in the derived class.
 */
void DevicePin::onPinInterrupt()
{}


/** Task function that calls a logic function when notified from ISR.
 *  `onInputObservationLoop` is designed as a FreeRTOS task function
 *  (i.e. with infinite loop and void pointer input). It runs and consumes
 *  CPU time only if it gets notified for hardware input changes by an
 *  interrupt service routine. Then, it calls the logic function
 *  `onInputObservation`.
 */
void DevicePin::onPinInterruptLoop(void* pvParameters)
{
  for(;;) {
    // Wait for a notification from an ISR
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //TODO
    this->onPinInterrupt();
    if(DEBUG) { //TODO
      PRINT("Debug: onPinInterruptLoop free stack size: "+String(
        uxTaskGetStackHighWaterMark(NULL)))
    }
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
  if(m_taskHandleOnValueChanged == NULL) {
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
  if(m_taskHandleOnPinInterrupt == NULL) {
    PRINT("Fatal: failed to create task onPinInterruptLoop")
  }
}
