#include "VehicleController.h"
#include "Device.h"


/** Registers a Parameter in the map.
 *  @param pParam pointer to a Parameter
 *  @return boolean whether the registration was successful
 */
bool VehicleController::registerParameter(Parameter* pParam)
{
  return m_map.addParameterToMap(pParam);
}


/** Registers a Device to a Parameter in the map.
 *  @param pCallingDevide pointer to the Device to register
 *  @param id integer ID of the Parameter to register it to
 *  @return boolean whether the registration was successful
 */
bool VehicleController::registerForValueChanged(Device* pCallingDevide, int id)
{
  return m_map.registerForValueChanged(pCallingDevide, id);
}


/** Unregisters a Device from a Parameter in the map.
 *  @param pCallingDevide pointer to the Device to unregister
 *  @param id integer ID of the Parameter to unregister it from
 *  @return boolean whether the unregistration was successful
 */
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


/** Sets a new value to a Parameter and notifies all registered Devices.
 *  @param pCallingDevide pointer to the Device that demands the Parameter value change
 *  @param pParam pointer to a boolean Parameter
 *  @param val new boolean value for the Parameter
 *  @return boolean whether Devices were notified successfully
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


/** Sets a new value to a Parameter and notifies all registered Devices.
 *  @param pCallingDevide pointer to the Device that demands the Parameter value change
 *  @param pParam pointer to a double Parameter
 *  @param val new double value for the Parameter
 *  @return boolean whether Devices were notified successfully
 */
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


/** Sets a new value to a Parameter and notifies all registered Devices.
 *  @param pCallingDevide pointer to the Device that demands the Parameter value change
 *  @param pParam pointer to a integer Parameter
 *  @param val new integer value for the Parameter
 *  @return boolean whether Devices were notified successfully
 */
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
