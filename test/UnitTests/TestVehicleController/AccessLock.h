/* ======================================================================
 * This is an automation for AccessControl. Creating an AccessLock at the
 * beginning of a scope will lock the given AccessControl and
 * automatically free it at the end of the scope.
 */

#ifndef ACCESSLOCK_H
#define ACCESSLOCK_H

#include "AccessControl.h"

class AccessLock
{
public:
	AccessLock(AccessControl* pMutex);
	~AccessLock();

private:
	AccessControl* m_pMutex;
};

#endif
