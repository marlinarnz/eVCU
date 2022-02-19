/* ======================================================================
 * The vehicle controller coordinates parameter changes and subscriptions.
 * It starts devices (services) and shuts them down, but does not
 * orchestrate anything.
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
	VehicleController() { m_map = MapParameterToDevices(); };
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
