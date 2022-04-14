#ifndef ACCESSCONTROL_H
#define ACCESSCONTROL_H

#include <Arduino.h>


/** An adapter for access controlling constructs like semaphores.
 *  Can be customised to the operating system used and other needs.
 */
class AccessControl
{
public:
  AccessControl();
  ~AccessControl();
  void lock();
  void unlock();

private:
  SemaphoreHandle_t m_handleMutex;
};

#endif
