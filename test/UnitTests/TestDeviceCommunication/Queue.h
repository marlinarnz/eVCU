#ifndef CUSTOM_QUEUE_H
#define CUSTOM_QUEUE_H

#include <Arduino.h>
#include "Constants.h"
#include "AccessControl.h"
#include "AccessLock.h"

class Parameter;


/** An adapter for queues that contain Parameters as messages.
 *  It is secured by a mutex. Can be customised to the operating
 *  system.
 */
class Queue
{
public:
  Queue();
  ~Queue();
  bool empty();
  Parameter* pop();
  Parameter* popWait(TickType_t waitingTicks);
  bool push(Parameter* pParam);

private:
  AccessControl m_mutex;
  QueueHandle_t m_handleQueue;
};

#endif
