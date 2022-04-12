#include "Queue.h"

Queue::Queue()
  : m_handleQueue(NULL), m_mutex(AccessControl())
{
  m_handleQueue = xQueueCreate(QUEUE_SIZE, 4);
  if (m_handleQueue==NULL) {
    if (DEBUG) {PRINT("Fatal: Queue generation failed")}
  }
}


Queue::~Queue()
{
  vQueueDelete(m_handleQueue);
  m_handleQueue = NULL;
}


bool Queue::empty()
{
  AccessLock lock(&m_mutex);
  Parameter* pParam;
  // Don't wait, if the element is not immediately available
  return !xQueuePeek(m_handleQueue, &(pParam), (TickType_t)0);
}


Parameter* Queue::pop()
{
  AccessLock lock(&m_mutex);
  Parameter* pParam = NULL;
  // Don't wait, if the element is not immediately available
  xQueueReceive(m_handleQueue, &(pParam), (TickType_t)0);
  return pParam;
}


bool Queue::push(Parameter* pParam)
{
  AccessLock lock(&m_mutex);
  // Wait 10 ticks if the queue is already full
  if (xQueueSend(m_handleQueue, (void*) &pParam, (TickType_t)10) != pdPASS) {
    return false;
  }
  return true;
}
