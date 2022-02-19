#include "Parameter.h"


Parameter::Parameter(int id)
  : m_id(id)
{}


Parameter::~Parameter() {}


int Parameter::getId()
{
  return m_id;
}


ParameterBool::ParameterBool(int id)
  : Parameter(id), m_phMutex(NULL), m_value(0)
{
  m_phMutex = new AccessControl();
}


ParameterBool::~ParameterBool() {
  delete m_phMutex;
}


void ParameterBool::setVal(bool val)
{
  AccessLock lock(m_phMutex);
  m_value = val;
}


bool ParameterBool::getVal()
{
  AccessLock lock(m_phMutex);
  return m_value;
}


ParameterInt::ParameterInt(int id)
  : Parameter(id), m_phMutex(NULL), m_value(0)
{
  m_phMutex = new AccessControl();
}


ParameterInt::~ParameterInt() {
  delete m_phMutex;
}


void ParameterInt::setVal(int val)
{
  AccessLock lock(m_phMutex);
  m_value = val;
}


int ParameterInt::getVal()
{
  AccessLock lock(m_phMutex);
  return m_value;
}


ParameterDouble::ParameterDouble(int id)
  : Parameter(id), m_phMutex(NULL), m_value(0)
{
  m_phMutex = new AccessControl();
}


ParameterDouble::~ParameterDouble() {
  delete m_phMutex;
}


void ParameterDouble::setVal(double val)
{
  AccessLock lock(m_phMutex);
  m_value = val;
}


double ParameterDouble::getVal()
{
  AccessLock lock(m_phMutex);
  return m_value;
}
