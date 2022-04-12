#include "VehicleController.h"
#include "Device.h"


bool VehicleController::registerParameter(Parameter* pParam)
{
  return m_map.addParameterToMap(pParam);
}


bool VehicleController::registerForValueChanged(Device* pCallingDevide, int id)
{
  return m_map.registerForValueChanged(pCallingDevide, id);
}


bool VehicleController::unregisterForValueChanged(Device* pCallingDevide, int id)
{
	return m_map.unregisterForValueChanged(pCallingDevide, id);
}

/*
bool VehicleController::unregisterForAll(Device* pCallingDevide)
{
	return m_map.unregisterDevice(pCallingDevide);
}
*/

bool VehicleController::setBooleanValue(Device* pCallingDevide, ParameterBool* pParam, bool val)
{
  // Check whether the value has changed
  if (pParam->getVal() != val) {
    pParam->setVal(val);
    // Notify all Devices that have subscribed to this parameter
    SecuredLinkedList<Device*>* devList = m_map.getRegisteredDevices(pParam->getId());
    if (devList) {
      for (int i=0; i<devList->size(); i++) {
        devList->get(i)->notifyValueChanged(pParam);
      }
      return true;
    }
  }
  return false;
}


bool VehicleController::setDoubleValue(Device* pCallingDevide, ParameterDouble* pParam, double val)
{
  // Check whether the value has changed
  if (pParam->getVal() != val) {
    pParam->setVal(val);
    // Notify all Devices that have subscribed to this parameter
    SecuredLinkedList<Device*>* devList = m_map.getRegisteredDevices(pParam->getId());
    if (devList) {
      for (int i=0; i<devList->size(); i++) {
        devList->get(i)->notifyValueChanged(pParam);
      }
      return true;
    }
  }
  return false;
}


bool VehicleController::setIntegerValue(Device* pCallingDevide, ParameterInt* pParam, int val)
{
  // Check whether the value has changed
  if (pParam->getVal() != val) {
    pParam->setVal(val);
    // Notify all Devices that have subscribed to this parameter
    SecuredLinkedList<Device*>* devList = m_map.getRegisteredDevices(pParam->getId());
    if (devList) {
      for (int i=0; i<devList->size(); i++) {
        devList->get(i)->notifyValueChanged(pParam);
      }
      return true;
    }
  }
  return false;
}
