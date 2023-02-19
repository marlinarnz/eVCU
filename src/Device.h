#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "AccessControl.h"
#include "Parameter.h"
#include "Queue.h"
#include "VehicleController.h"


/** Base class for all services of a vehicle.
 *  A service can do the following:
 * - observe its inputs using hardware interrupts
 * - register for value changes of other devices' parameters (known by ID)
 * - unregister for value changes
 * - execute a logic on external Parameter value changes
 * Devices can run two tasks: Process notifications from other devices;
 * and process hardware interrupts and notify other devices. The
 * base class provides default definition of the first task.
 */
class Device
{
  friend class VehicleController;

public:
  Device(VehicleController* pController);
  ~Device();
  virtual void begin() = 0;                           // To be defined in derived class, must call startTasks()
  virtual void shutdown() = 0;                        // To be defined in derived class

private:
  virtual void onValueChanged(Parameter* pParam);     // Can be defined in derived class
  
  VehicleController* m_pController;
  Queue m_paramsQueue;
  
  void onValueChangedLoop(void* pvParameters);
  void notifyValueChanged(Parameter* pParamWithNewValue);

protected:
  // Functions to be used by derived classes
  bool registerParameter(int id);
  bool registerForValueChanged(int id);
  bool unregisterForValueChanged(int id);
  //DEPRECTED: bool unregisterForAll() { return m_pController->unregisterForAll(this); }
  bool setBooleanValue(ParameterBool* pParam, bool bValue);
  bool setDoubleValue(ParameterDouble* pParam, double dValue);
  bool setIntegerValue(ParameterInt* pParam, int nValue);
  
  virtual void startTasks(uint16_t stackSize=4096);    // Can be defined in derived class
  TaskHandle_t m_taskHandleOnValueChanged;
  static void startOnValueChangedLoop(void* _this);
};

#endif
