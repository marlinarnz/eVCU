#include "MapParameterToDevices.h"


bool MapParameterToDevices::addParameterToMap(Parameter* pNewParam)
{
  // See whether the parameter is already registered and break in this case
  if (this->has(pNewParam->getId())) {
    if (DEBUG) {PRINT("Parameter already added with ID " + String(pNewParam->getId()))}
    return false;
  }
  else {
    // Create the new list on the heap
    SecuredLinkedList<Device*>* devicesList = new SecuredLinkedList<Device*>();
    // Add it to the end of this list
    this->put(pNewParam->getId(), devicesList);
    return true;
  }
}


bool MapParameterToDevices::registerForValueChanged(Device* pCallingDevice, int id)
{
  // Find the given ID and add the device to its list
  if (!this->has(id)) {
    if (DEBUG) {PRINT("No parameter found with ID " + String(id))}
    return false;
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

SecuredLinkedList<Device*>* MapParameterToDevices::getRegisteredDevices(int id)
{
  // Return a pointer to the device list of the given ID
  if (!this->has(id)) {
    // Return NULL if the ID was not found
    return NULL;
  }
  return this->get(id);
}


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
