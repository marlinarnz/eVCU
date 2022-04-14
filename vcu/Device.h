/* ======================================================================
 * This is the abstract class for all services of the vehicle, including
 * the vehicle itself. A service can do the following:
 * - observe its inputs using hardware interrupts
 * - register for value changes of other devices' parameters (known by ID)
 * - unregister for value changes
 * - execute a logic on external Parameter value changes
 * Each input observation is a task that waits for an event in order to
 * run and notify other devices of this parameter change. These tasks
 * are started in the begin() function.
 */

#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "AccessControl.h"
#include "Parameter.h"
#include "Queue.h"
#include "VehicleController.h"

class Device
{
  friend class VehicleController;

public:
  Device(VehicleController* pController)
  : m_pController(pController), m_paramsQueue(Queue()), taskHandleOnValueChanged(NULL),
    taskHandleOnInputObservation(NULL) { startTasks(); };
  ~Device() {vTaskDelete(taskHandleOnValueChanged); vTaskDelete(taskHandleOnInputObservation);};
  void begin(); // Assign events to Parameters
  void shutdown(); // Reset everything back to default

private:
  void onValueChanged(Parameter* pParamWithNewValue) {}; // Logic for handling Parameter changes
  void onInputObservation() {}; // Logic for handling hardware interrupts
  
  VehicleController* m_pController;
  Queue m_paramsQueue;
  
  TaskHandle_t taskHandleOnValueChanged;
  void onValueChangedLoop(void* pvParameters) {
    for(;;) {
      // Wait infinitely to get notified that the queue was filled
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
      // Process parameter changes until the queue is empty
      while(!m_paramsQueue.empty()) {
        this->onValueChanged(m_paramsQueue.pop());
      }
    }
  };
  void notifyValueChanged(Parameter* pParamWithNewValue)
  {
    if (m_paramsQueue.push(pParamWithNewValue)) {
      xTaskNotifyGive(taskHandleOnValueChanged);
      if(DEBUG) {PRINT("Debug: onValueChangedLoop free stack size: "+String(uxTaskGetStackHighWaterMark(taskHandleOnValueChanged)))}
    } else {
      PRINT("Error: failed to push parameter to queue")
    }
  };
  static void startOnValueChangedLoop(void* _this) { static_cast<Device*>(_this)->onValueChangedLoop(NULL); };

  void onInputObservationLoop(void* pvParameters) {
    for(;;) {
      // Wait for a notification from an ISR
      uint32_t ulNotificationValue;
      const TickType_t xMaxBlockTime = pdMS_TO_TICKS(10000);
      ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);

      if(ulNotificationValue == 1) {
        this->onInputObservation();
      } else {
        PRINT("Debug: no input value change observed for 10 seconds")
      }
      if(DEBUG) {PRINT("Debug: handleSerialInput free stack size: "+String(uxTaskGetStackHighWaterMark(taskHandleOnInputObservation)))}
    }
  };
  static void startOnInputObservationLoop(void* _this) { static_cast<Device*>(_this)->onInputObservationLoop(NULL); };

  void startTasks() {
    xTaskCreatePinnedToCore(
      this->startOnValueChangedLoop, // function name
      "onValueChanged", // Name for debugging
      1000, // Stack size in words
      this, // Parameters pointer for the function; must be static
      1, // Priority (1 is lowest)
      &taskHandleOnValueChanged, // task handle
      1 // CPU core
    );
    if(taskHandleOnValueChanged == NULL) {
      PRINT("Fatal: failed to create task onValueChanged")
    }
    
    xTaskCreatePinnedToCore(
      this->startOnInputObservationLoop, // function name
      "onInput", // Name for debugging
      1000, // Stack size in words
      this, // Parameters pointer for the function; must be static
      1, // Priority (1 is lowest)
      &taskHandleOnInputObservation, // task handle
      1 // CPU core
    );
    if(taskHandleOnInputObservation == NULL) {
      PRINT("Fatal: failed to create task onInputObservation")
    }
  };

protected:
  // Functions to be used by derived classes
  bool registerParameter(Parameter* pParam) { return m_pController->registerParameter(pParam); }
  bool registerForValueChanged(int id) { return m_pController->registerForValueChanged(this, id); }
  bool unregisterForValueChanged(int id) { return m_pController->unregisterForValueChanged(this, id); }
  //DEPRECTED: bool unregisterForAll() { return m_pController->unregisterForAll(this); }
  bool setBooleanValue(ParameterBool* pParam, bool bValue) { return m_pController->setBooleanValue(this, pParam, bValue); }
  bool setDoubleValue(ParameterDouble* pParam, double dValue) { return m_pController->setDoubleValue(this, pParam, dValue); }
  bool setIntegerValue(ParameterInt* pParam, int nValue) { return m_pController->setIntegerValue(this, pParam, nValue); }
  TaskHandle_t taskHandleOnInputObservation;
};

#endif
