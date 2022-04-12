/* ======================================================================
 * The vehicle controller registers subscriptions of Devices to Parameter
 * changes. It forwards these Parameter changes to the registered Devices
 * but does not execute any further logic.
 */

#ifndef VEHICLECONTROLLER_H
#define VEHICLECONTROLLER_H

#include <Arduino.h>
#include "MapParameterToDevices.h"

class Device;
class Parameter;

class VehicleController
{
public:
	VehicleController() : m_map(MapParameterToDevices()) {};
	~VehicleController() {};
  bool registerParameter(Parameter* pParam);
  bool registerForValueChanged(Device* pCallingDevide, int id);
	bool unregisterForValueChanged(Device* pCallingDevide, int id);
	//bool unregisterForAll(Device* pCallingDevide);
	bool setBooleanValue(Device* pCallingDevide, ParameterBool* pParam, bool bValue);
	bool setDoubleValue(Device* pCallingDevide, ParameterDouble* pParam, double dValue);
	bool setIntegerValue(Device* pCallingDevide, ParameterInt* pParam, int nValue);

private:
	MapParameterToDevices m_map;
};

#endif
