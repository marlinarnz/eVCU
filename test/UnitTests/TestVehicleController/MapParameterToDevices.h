/* ======================================================================
 * This map-like class is based on the SecuredLinkedList by Steven Cybinski.
 * It is a list of elements, which contain one Parameter and a list of
 * Devices, which are registered for its changes. The element can be
 * identified by the Parameter's ID.
 */

#ifndef MAPPARAMETERTODEVICES_H
#define MAPPARAMETERTODEVICES_H

#include <Arduino.h>
#include "Parameter.h"
#include "SecuredLinkedList.h"
#include "SecuredLinkedListMap.h"
#include "Constants.h"
#include "Device.h"


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
