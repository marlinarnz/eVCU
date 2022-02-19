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
  Parameter(int id);
  ~Parameter();
  int getId();
private:
  int m_id;
};


class ParameterBool : public Parameter
{
public:
  ParameterBool(int id);
  ~ParameterBool();
  void setVal(bool val);
  bool getVal();
private:
  bool m_value;
  AccessControl* m_phMutex;
};


class ParameterDouble : public Parameter
{
public:
  ParameterDouble(int id);
  ~ParameterDouble();
  void setVal(double val);
  double getVal();
private:
  double m_value;
  AccessControl* m_phMutex;
};


class ParameterInt : public Parameter
{
public:
  ParameterInt(int id);
  ~ParameterInt();
  void setVal(int val);
  int getVal();
private:
  int m_value;
  AccessControl* m_phMutex;
};

#endif
