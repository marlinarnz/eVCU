#ifndef MAPPARAMETERTODEVICES_H
#define MAPPARAMETERTODEVICES_H

#include <Arduino.h>
#include "Constants.h"
#include "Parameter.h"
#include "SecuredLinkedList.h"
#include "SecuredLinkedListMap.h"

class Device;


/** A map to link Parameters to a list of Devices.
 * This map-like class is based on the SecuredLinkedList by Steven Cybinski.
 * It is a map of Parameter IDs and a list of Devices, which are registered
 * for the Parameter's changes. This class does not execute any further 
 * logic.
 */
class MapParameterToDevices : public SecuredLinkedListMap<int, SecuredLinkedList<Device*>*>
{
public:
  MapParameterToDevices() {};
  ~MapParameterToDevices() {};
  bool addParameterToMap(Parameter* pNewParam);
  bool registerForValueChanged(Device* pCallingDevice, int id);
  bool unregisterForValueChanged(Device* pCallingDevice, int id);
  //bool unregisterDevice(Device* pCallingDevice);
  SecuredLinkedList<Device*>* getRegisteredDevices(int id);

private:
  bool removeDeviceFromList(SecuredLinkedList<Device*>* pDevicesList, Device* pDevice);
};

#endif
