#include "MapParameterToDevices.h"


/** Registers a new Parameter in the map.
 *  If the ID of the given Parameter has not been registered yet,
 *  it gets a new entry with empty list of registered Devices.
 *  @param id unique identifier of the new Parameter
 *  @return boolean whether the Parameter was registered successfully or already exists
 */
bool MapParameterToDevices::addParameterToMap(int id)
{
  // See whether the parameter is already registered and break in this case
  if (this->has(id)) {
    if (DEBUG) {PRINT("Parameter already added with ID " + String(id))}
    return false;
  }
  else {
    // Create the new list on the heap
    SecuredLinkedList<Device*>* devicesList = new SecuredLinkedList<Device*>();
    // Add it to the end of this list
    this->put(id, devicesList);
    return true;
  }
}


/** Registers a Device for a Parameter.
 *  If the Parameter ID to register for is present in the map,
 *  the Device is registered in the notification list.
 *  @param pCallingDevice pointer to the Device to register
 *  @param id integer ID of the Parameter to register the Device for
 *  @return boolean whether the Device was registered successfully
 */
bool MapParameterToDevices::registerForValueChanged(Device* pCallingDevice, int id)
{
  // Find the given ID and add the device to its list
  if (!this->has(id)) {
    if (DEBUG) {PRINT("Registering parameter with ID " + String(id))}
    if (!this->addParameterToMap(id)) return false;
  }
  // See whether the device is already registered
  for (unsigned int i=0; i<this->get(id)->size(); i++) {
    if (this->get(id)->get(i) == pCallingDevice) {
      if (DEBUG) {PRINT("Device already registered for parameter with ID " + String(id))}
      return false;
    }
  }
  // If not, register it
  this->get(id)->push(pCallingDevice);
  return true;
}


/** Unregisters a Device for a Parameter.
 *  If the Device was registered to this Parameter, it gets
 *  unregistered from the notification list.
 *  @param pCallingDevice pointer to the Device to unregister
 *  @param id integer ID of the Parameter to unregister the Device from
 *  @return boolean whether the Device was unregistered successfully
 */
bool MapParameterToDevices::unregisterForValueChanged(Device* pCallingDevice, int id)
{
  // Find the given ID and remove the device there
  if (!this->has(id)) {
    return false;
  }
  return MapParameterToDevices::removeDeviceFromList(this->get(id), pCallingDevice);
}

/*
bool MapParameterToDevices::unregisterDevice(Device* pCallingDevice)
{
  // Remove device from all elements of this list
  int boolSum = 0;
  SecuredLinkedListMapElement<int, SecuredLinkedList<Device*>*> mapCopy[this->size()];
  for (int i=0; i<this->size(); i++) {
    bool returnVal = MapParameterToDevices::removeDeviceFromList(mapCopy[i].value, pCallingDevice);
    boolSum = boolSum + returnVal;
  }
  // Return if the device was removed at least once
  if (boolSum > 0) {
    return true;
  } else {
    return false;
  }
}
*/


/** Get the list of registered Devices for a Parameter.
 *  If the Parameter ID is registered in the map, return a list
 *  of Devices that are registered to its value changes.
 *  @param id integer ID of the Parameter
 *  @return SecuredLinkedList of pointers to registered Devices
 */
SecuredLinkedList<Device*>* MapParameterToDevices::getRegisteredDevices(int id)
{
  // Return a pointer to the device list of the given ID
  if (!this->has(id)) {
    // Return NULL if the ID was not found
    return NULL;
  }
  return this->get(id);
}


/** Removes a Device from a list.
 *  
 *  @param pDevicesList pointer to the list
 *  @param pDevice pointer to the Device to remove
 *  @return boolean whether the Device was removed successfully
 */
bool MapParameterToDevices::removeDeviceFromList(SecuredLinkedList<Device*>* pDevicesList, Device* pDevice)
{
  // Search the given list for the given device
  for (unsigned int i=0; i<pDevicesList->size(); i++) {
    Device* pDev = pDevicesList->get(i);
    // Compare the pointers
    if (pDev == pDevice) {
      // Pop that device: To do so, pop all devices above it or all below it
      if (i == pDevicesList->size()-1) {
        Device* trash = pDevicesList->pop();
      } else if (i == 0) {
        Device* trash = pDevicesList->shift();
      } else {
        // Pop everything above and add it again
        int head = pDevicesList->size() - i - 1;
        Device* tempStorage[head];
        for (unsigned int j=0; j<head; j++) {
          tempStorage[j] = pDevicesList->pop();
        }
        Device* trash = pDevicesList->pop();
        for (unsigned int j=0; j<head; j++) {
          pDevicesList->push(tempStorage[j]);
        }
      }
      // Return success
      return true;
    }
  }
  // Return false, if nothing happened
  if (DEBUG) {PRINT("No device found to unregister")}
  return false;
}
