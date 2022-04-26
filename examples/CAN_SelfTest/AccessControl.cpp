#include "AccessControl.h"


/** The constructor instantiates the mutex.
 */
AccessControl::AccessControl()
  : m_handleMutex(NULL)
{
  m_handleMutex = xSemaphoreCreateMutex();
}


/** The destructor deletes the mutex.
 */
AccessControl::~AccessControl()
{
  vSemaphoreDelete(m_handleMutex);
}


/** Locks the mutex.
 */
void AccessControl::lock()
{
  xSemaphoreTake(m_handleMutex, portMAX_DELAY);
}


/** Unlocks the mutex.
 */
void AccessControl::unlock()
{
  xSemaphoreGive(m_handleMutex);
}
