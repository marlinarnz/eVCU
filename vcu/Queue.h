/* ======================================================================
 * An adapter for queues. Can be customised to the operating system used
 * and other needs such as return type.
 */

#ifndef CUSTOM_QUEUE_H
#define CUSTOM_QUEUE_H

#include <Arduino.h>
#include "Constants.h"
#include "AccessControl.h"
#include "AccessLock.h"

class Parameter;

class Queue
{
public:
  Queue();
  ~Queue();
  bool empty();
  Parameter* pop();
  bool push(Parameter* pParam);

private:
  AccessControl m_mutex;
  QueueHandle_t m_handleQueue;
};

#endif
