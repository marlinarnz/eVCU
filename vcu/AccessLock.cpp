#include "AccessLock.h"


/** The constructor locks the given mutex.
 */
AccessLock::AccessLock(AccessControl* pMutex)
  : m_pMutex(pMutex)
{
	m_pMutex->lock();
}


/** The destructor unlocks the mutex.
 */
AccessLock::~AccessLock()
{
	m_pMutex->unlock();
}
