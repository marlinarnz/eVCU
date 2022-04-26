#include "DeviceSerial.h"


/** The constructor.
 */
DeviceSerial::DeviceSerial(VehicleController* pController)
  : Device(pController), m_taskHandleOnSerialEvent(NULL)
{}


/** The destructor deletes tasks.
 */
DeviceSerial::~DeviceSerial()
{
  vTaskDelete(m_taskHandleOnSerialEvent);
}


/** Driver-specific function to wait for serial inputs.
 *  Must be filled in the derived class. Calls `onSerialEvent()`.
 */
void DeviceSerial::waitForSerialEvent()
{}


/** Task function that loops a wait function.
 *  `onSerialEventLoop` is designed as a FreeRTOS task function
 *  (i.e. with infinite loop and void pointer input). It runs 
 *  `waitForSerialEvent()` which waits for an event on the serial bus.
 *  Then, it calls the logic function `onSerialEvent()`.
 */
void DeviceSerial::onSerialEventLoop(void* pvParameters)
{
  for(;;) {
    // Wait for an event on the serial bus, depending on the derived class
    this->waitForSerialEvent();
    /*if (DEBUG) {
      PRINT("Debug: onSerialEventLoop free stack size: "+String(
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
void DeviceSerial::startOnSerialEventLoop(void* _this)
{
  static_cast<DeviceSerial*>(_this)->onSerialEventLoop(NULL);
}


/** Starts tasks for processing value changes and hardware inputs.
 *  Must be called in the derived class's `begin()` function. FreeRTOS
 *  tasks use stack sizes in words (i.e. stack depth), which
 *  corresponds to 1/4th of the stack size in bytes on 32bit-systems.
 *  @param stackSizeOnValueChanged size of onValueChangedLoop task
 *         stack in bytes. Default is 4096
 *  @param stackSizeOnPinInterrupt size of onPinInterruptLoop task
 *         stack in bytes. Default is 4096
 */
void DeviceSerial::startTasks(uint16_t stackSizeOnValueChanged,
                              uint16_t stackSizeOnSerialEvent)
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
    this->startOnSerialEventLoop, // function name
    "onSerialEvent", // Name for debugging
    (uint16_t)(stackSizeOnSerialEvent/4),
    this, // Parameters pointer for the function; must be static
    1, // Priority (1 is lowest)
    &m_taskHandleOnSerialEvent, // task handle
    1 // CPU core
  );
  if (m_taskHandleOnSerialEvent == NULL) {
    PRINT("Fatal: failed to create task onSerialEventLoop")
  }
}
