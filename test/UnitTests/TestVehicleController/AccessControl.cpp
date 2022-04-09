#include "AccessControl.h"

AccessControl::AccessControl()
  : m_handleMutex(NULL)
{
  m_handleMutex = xSemaphoreCreateMutex();
}


AccessControl::~AccessControl()
{
  vSemaphoreDelete(m_handleMutex);
}


void AccessControl::lock()
{
  xSemaphoreTake(m_handleMutex, portMAX_DELAY);
}


void AccessControl::unlock()
{
  xSemaphoreGive(m_handleMutex);
}
