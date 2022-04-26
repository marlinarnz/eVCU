#include "Parameter.h"


void ParameterBool::setVal(bool val)
{
  AccessLock lock(&m_pMutex);
  m_value = val;
}


bool ParameterBool::getVal()
{
  AccessLock lock(&m_pMutex);
  return m_value;
}


void ParameterInt::setVal(int val)
{
  AccessLock lock(&m_pMutex);
  m_value = val;
}


int ParameterInt::getVal()
{
  AccessLock lock(&m_pMutex);
  return m_value;
}


void ParameterDouble::setVal(double val)
{
  AccessLock lock(&m_pMutex);
  m_value = val;
}


double ParameterDouble::getVal()
{
  AccessLock lock(&m_pMutex);
  return m_value;
}
