/* ======================================================================
 * An adapter for queues. Can be customised to the operating system used
 * and other needs such as return type.
 */

#ifndef CUSTOM_QUEUE_H
#define CUSTOM_QUEUE_H

#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
//#include <queue.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/queue.h"
#include "Constants.h"
#include "AccessControl.h"
#include "AccessLock.h"

class Parameter;

class Queue
{
public:
  Queue(size_t sizeOfElement);
  ~Queue();
  bool empty();
  Parameter* pop();
  bool push(Parameter* pParam);

private:
  AccessControl* m_phMutex;
  QueueHandle_t m_handleQueue;
};

#endif
