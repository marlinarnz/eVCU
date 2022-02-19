/* ======================================================================
 * An adapter for access controlling objects like semaphores. Can be
 * customised to the operating system used and other needs.
 */

#ifndef ACCESSCONTROL_H
#define ACCESSCONTROL_H

#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/semphr.h"

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
