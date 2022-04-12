/* ======================================================================
 * This is the abstract class for all services of the vehicle, including
 * the vehicle itself. A service can do the following:
 * - observe its inputs (event-driven)
 * - register for value changes of other devices' parameters (known by ID)
 * - unregister for value changes
 * - execute a logic on value changes
 * Each input observation is a task that waits for an event in order to
 * run and notify other devices of this parameter change. These tasks
 * are implemented in the begin() function.
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
  Device(VehicleController* pController) : m_pController(pController), m_paramsQueue(Queue()) {};
  ~Device() {};
  void begin(); // Assign events to Parameters
  void shutdown(); // Reset everything back to default

private:
  void onValueChanged(Parameter* pParamWithNewValue) {}; // Logic for handling Parameter changes
  VehicleController* m_pController;
  Queue m_paramsQueue;

protected:
  void notifyValueChanged(Parameter* pParamWithNewValue)
  {
    if (m_paramsQueue.push(pParamWithNewValue)) {
      onValueChanged(m_paramsQueue.pop());
    } else {
      PRINT("Error: railed to push parameter to queue")
    }
  };
  bool registerParameter(Parameter* pParam) { return m_pController->registerParameter(pParam); }
  bool registerForValueChanged(int id) { return m_pController->registerForValueChanged(this, id); }
  bool unregisterForValueChanged(int id) { return m_pController->unregisterForValueChanged(this, id); }
  //DEPRECTED: bool unregisterForAll() { return m_pController->unregisterForAll(this); }
  bool setBooleanValue(ParameterBool* pParam, bool bValue) { return m_pController->setBooleanValue(this, pParam, bValue); }
  bool setDoubleValue(ParameterDouble* pParam, double dValue) { return m_pController->setDoubleValue(this, pParam, dValue); }
  bool setIntegerValue(ParameterInt* pParam, int nValue) { return m_pController->setIntegerValue(this, pParam, nValue); }
};

#endif
