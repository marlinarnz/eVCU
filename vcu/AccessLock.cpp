#include "AccessLock.h"

AccessLock::AccessLock(AccessControl* pMutex)
  : m_pMutex(pMutex)
{
	m_pMutex->lock();
}


AccessLock::~AccessLock()
{
	m_pMutex->unlock();
}
