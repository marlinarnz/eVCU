/* ======================================================================
 * This is the abstract class for all services of the vehicle, including
 * the vehicle itself. A service can do the following:
 * - observe its inputs
 * - register for value changes of vehicle parameters (known by ID)
 * - unregister
 * - execute a logic on value changes
 * Logic and input observation are different threads. An event queue
 * coordinates values changes.
 */

#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include "AccessControl.h"
#include "VehicleController.h"
#include "Parameter.h"
#include "Queue.h"

class Device
{
	friend class VehicleController;

public:
	Device(VehicleController* pController) : m_pController(pController), m_paramsQueue(NULL), m_queueMutex(NULL) { };
	  //TODO: object init list
    // create queue with new mutex
	~Device();
	virtual void init();
	virtual void run();
	virtual void shutdown();

private:
  virtual bool onValueChanged(Parameter* pParamWithNewValue);
  VehicleController* m_pController;
  Queue m_paramsQueue;
  AccessControl m_queueMutex;

protected:
	void notifyValueChanged(Parameter* pParamWithNewValue);
	//TODO: m_paramsQueue.push(pParamWithNewValue);
	// call: onValueChanged(m_paramsQueue.pop());
	bool registerParameter(Parameter* pParam) { return m_pController->registerParameter(pParam); }
	bool registerForValueChanged(int id) { return m_pController->registerForValueChanged(this, id); }
	bool unregisterForValueChanged(int id) { return m_pController->unregisterForValueChanged(this, id); }
	bool unregisterForAll() { return m_pController->unregisterForAll(this); }
	bool setBooleanValue(int id, bool bValue) { return m_pController->setBooleanValue(this, id, bValue); }
	bool setDoubleValue(int id, double dValue) { return m_pController->setDoubleValue(this, id, dValue); }
	bool setIntegerValue(int id, int nValue) { return m_pController->setIntegerValue(this, id, nValue); }
};

#endif
