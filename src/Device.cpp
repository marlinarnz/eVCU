#include "Device.h"


/** The constructor initialises the object's members and tasks.
 *  Derived classes must call the parent constructor like so:
 *  `DeviceChild(VehicleController* vc) : Device(vc) {}`
 *  @param pController pointer to the global VehicleController instance
 */
Device::Device(VehicleController* pController)
  : m_pController(pController), m_paramsQueue(Queue()), m_taskHandleOnValueChanged(NULL)
{}


/** The destructor deletes all of the object's tasks.
 */
Device::~Device()
{
  vTaskDelete(m_taskHandleOnValueChanged);
}


/** Logic function to handle Parameter value changes.
 *  This function must be filled in the derived class, if it will
 *  process Parameter changes. A switch-case statement is useful here.
 *  @param pParamWithNewValue pointer to a Parameter to be processed
 */
void Device::onValueChanged(Parameter* pParamWithNewValue)
{
  /*if (pParamWithNewValue) {
    switch(pParamWithNewValue->getId()) {
      default:
        break;
    }
  }*/
}


/** Registers this Device for Parameter changes in the VehicleController.
 *  Derived classes should use this function in order to get
 *  messages from other Devices.
 *  @param id integer ID of the Parameter to subscribe to
 *  @return boolean if the VehicleController was successful
 */
bool Device::registerForValueChanged(int id)
{
  return m_pController->registerForValueChanged(this, id);
}


/** Unregisters this Device for Parameter changes in the VehicleController.
 *  Derived classes should use this function, if they don't want
 *  notifications about this Parameter anymore.
 *  @param id integer ID of the Parameter to unsubscribe from
 *  @return boolean if the VehicleController was successful
 */
bool Device::unregisterForValueChanged(int id)
{
  return m_pController->unregisterForValueChanged(this, id);
}


/** Sets a new boolean value to a Parameter and notifies other Devices.
 *  Derived classes should use this function in order to communicate
 *  with other Devices.
 *  @param pParam pointer to a Parameter that will get a new value
 *  @param bValue double value to set the Parameter
 *  @return boolean if the VehicleController was successful
 */
bool Device::setBooleanValue(ParameterBool* pParam, bool bValue)
{
  return m_pController->setBooleanValue(this, pParam, bValue);
}


/** Sets a new integer value to a Parameter and notifies other Devices.
 *  Derived classes should use this function in order to communicate
 *  with other Devices.
 *  @param pParam pointer to a Parameter that will get a new value
 *  @param nValue integer value to set the Parameter
 *  @return boolean if the VehicleController was successful
 */
bool Device::setIntegerValue(ParameterInt* pParam, int nValue)
{
  return m_pController->setIntegerValue(this, pParam, nValue);
}


/** Sets a new double value to a Parameter and notifies other Devices.
 *  Derived classes should use this function in order to communicate
 *  with other Devices.
 *  @param pParam pointer to a Parameter that will get a new value
 *  @param dValue boolean value to set the Parameter
 *  @return boolean if the VehicleController was successful
 */
bool Device::setDoubleValue(ParameterDouble* pParam, double dValue)
{
  return m_pController->setDoubleValue(this, pParam, dValue);
}


/** Registers a Parameter in the VehicleController.
 *  This function can be used, if the Parameter cannot be registered
 *  using the VehicleController instance directly. A registered
 *  Parameter can be subscribed to by other Devices.
 *  @param pParam pointer to a Parameter that should be accessible to other Devices
 *  @return boolean if the VehicleController was successful
 */
bool Device::registerParameter(Parameter* pParam)
{
  return m_pController->registerParameter(pParam);
}


/** Task function that calls a logic function when notified for parameter changes.
 *  `onValueChangedLoop` is designed as a FreeRTOS task function
 *  (i.e. with infinite loop and void pointer input). It runs and consumes
 *  CPU time only if it gets notified. Then, it processes the messages
 *  in the queue by calling the logic function `onValueChanged`.
 */
void Device::onValueChangedLoop(void* pvParameters)
{
  for(;;) {
    // Process parameter changes until the queue is empty
    this->onValueChanged(m_paramsQueue.popWait(portMAX_DELAY)); //TODO
    /*if (DEBUG) {
      PRINT("Debug: onValueChangedLoop free stack size: "+String(
        uxTaskGetStackHighWaterMark(NULL)))
    }*/
  }
  vTaskDelete(NULL);
}


/** Used to notify the Device of Parameter value changes.
 *  The VehicleController calls this function, if the Device has
 *  registered itself for this Parameter, which then puts the given
 *  Parameter pointer into the queue for changes to be processed.
 *  Then it notifies the value change processing task.
 *  @param pParamWithNewValue pointer to a Parameter to be processed
 */
void Device::notifyValueChanged(Parameter* pParamWithNewValue)
{
  if (m_paramsQueue.push(pParamWithNewValue) != true) {
    PRINT("Error: failed to push parameter to queue")
  }
}


/** Static adaptor for non-static member function.
 *  FreeRTOS tasks work with static member functions only. This
 *  worksaround has the drawback that tasks must be created
 *  by the corresponding object itself.
 *  @param _this this pointer
 */
void Device::startOnValueChangedLoop(void* _this)
{
  static_cast<Device*>(_this)->onValueChangedLoop(NULL);
}


/** Starts tasks for processing value changes.
 *  Must be called in the derived class's begin() function.
 *  Derived classes can override this function and add tasks
 *  for hardware interrupt handling. FreeRTOS tasks use
 *  stack sizes in words (i.e. stack depth), which corresponds
 *  to 1/4th of the stack size in bytes on 32bit-systems.
 *  @param stackSize size of task stack in bytes. Default is 4096
 */
void Device::startTasks(uint16_t stackSize)
{
  xTaskCreatePinnedToCore(
    this->startOnValueChangedLoop, // function name
    "onValueChanged", // Name for debugging
    (uint16_t)(stackSize/4), // Stack size in words
    this, // Parameters pointer for the function; must be static
    1, // Priority (1 is lowest)
    &m_taskHandleOnValueChanged, // task handle
    1 // CPU core
  );
  if (m_taskHandleOnValueChanged == NULL) {
    PRINT("Fatal: failed to create task onValueChanged")
  }
}
