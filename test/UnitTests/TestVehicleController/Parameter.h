/* ======================================================================
 * A Parameter can hold one value of the respective data type. It needs
 * a mutex that is called when the value is set or retrieved.
 */

#ifndef PARAMETER_H
#define PARAMETER_H

#include <Arduino.h>
#include "AccessControl.h"
#include "AccessLock.h"

class Parameter
{
public:
  AccessControl m_phMutex;
  
  Parameter(int id) : m_phMutex(AccessControl()), m_id(id) {};
  int getId() {return m_id;};

private:
  int m_id;
};


class ParameterBool : public Parameter
{
public:
  ParameterBool(int id) : Parameter(id), m_value(0) {};
  void setVal(bool val);
  bool getVal();
private:
  bool m_value;
  
};


class ParameterDouble : public Parameter
{
public:
  ParameterDouble(int id) : Parameter(id), m_value(0) {};
  void setVal(double val);
  double getVal();
private:
  double m_value;
};


class ParameterInt : public Parameter
{
public:
  ParameterInt(int id) : Parameter(id), m_value(0) {};
  void setVal(int val);
  int getVal();
private:
  int m_value;
};

#endif
