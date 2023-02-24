#include "DeviceLoop.h"


/** The constructor attaches the interrupt.
 *  @param pController: VehicleController instance passed to Device
 *  @param loopTime: delay time (ms) of the loop
 */
DeviceLoop::DeviceLoop(VehicleController* pController, int loopTime)
  : Device(pController), m_taskHandleLoop(NULL),
    m_loopTime(loopTime)
{}


/** The destructor deletes tasks.
 */
DeviceLoop::~DeviceLoop()
{
  if(m_taskHandleLoop != NULL){
    vTaskDelete(m_taskHandleLoop);
    m_taskHandleLoop = NULL;
  }
}


/** Logic function to execute in every loop iteration.
 *  This function must be filled in the derived class.
 */
void DeviceLoop::onLoop()
{}


/** Task function that iterates a loop.
 *  It calls the logic function `onLoop` and delays `m_loopTime`.
 */
void DeviceLoop::onLoopLoop(void* pvParameters)
{
  for(;;) {
    this->onLoop();
    vTaskDelay(this->m_loopTime);
  }
  vTaskDelete(NULL);
}


/** Static adaptor for non-static member function.
 *  FreeRTOS tasks work with static member functions only. This
 *  worksaround has the drawback that tasks must be created by the
 *  corresponding object itself.
 *  @param _this this pointer
 */
void DeviceLoop::startLoopLoop(void* _this)
{
  static_cast<DeviceLoop*>(_this)->onLoopLoop(NULL);
}


/** Starts tasks for processing value changes and hardware inputs.
 *  Must be called in the derived class's begin() function. FreeRTOS
 *  tasks use stack sizes in words (i.e. stack depth), which
 *  corresponds to 1/4th of the stack size in bytes on 32bit-systems.
 *  @param stackSizeOnValueChanged size of onValueChangedLoop task
 *         stack in bytes. Default is 4096
 *  @param stackSizeLoop size of onPinInterruptLoop task
 *         stack in bytes. Default is 4096
 */
void DeviceLoop::startTasks(uint16_t stackSizeOnValueChanged,
                            uint16_t stackSizeLoop)
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
    this->startLoopLoop, // function name
    "onLoop", // Name for debugging
    (uint16_t)(stackSizeLoop/4),
    this, // Parameters pointer for the function; must be static
    1, // Priority (1 is lowest)
    &m_taskHandleLoop, // task handle
    1 // CPU core
  );
  if (m_taskHandleLoop == NULL) {
    PRINT("Fatal: failed to create task onLoopLoop")
  }
}
