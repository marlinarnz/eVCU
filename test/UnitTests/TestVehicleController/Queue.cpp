#include "Queue.h"

Queue::Queue(size_t sizeOfElement)
  : m_handleQueue(NULL), m_phMutex(NULL)
{
  m_phMutex = new AccessControl();
  m_handleQueue = xQueueCreate(QUEUE_SIZE, sizeOfElement);
  if (m_handleQueue==NULL) {
    if (DEBUG) {PRINT("Fatal: Queue generation failed")}
  }
}


Queue::~Queue()
{
  delete m_phMutex;
  vQueueDelete(m_handleQueue);
  m_handleQueue = NULL;
}


bool Queue::empty()
{
  AccessLock lock(m_phMutex);
  Parameter* element;
  // Don't wait, if the element is not immediately available
  return !xQueuePeek(m_handleQueue, &element, (TickType_t)0);
}


Parameter* Queue::pop()
{
  AccessLock lock(m_phMutex);
  Parameter* element = NULL;
  // Don't wait, if the element is not immediately available
  xQueueReceive(m_handleQueue, &element, (TickType_t)0);
  return element;
}


bool Queue::push(Parameter* pParam)
{
  AccessLock lock(m_phMutex);
  // Wait 10 ticks if the queue is already full
  if (xQueueSend(m_handleQueue, &pParam, (TickType_t)10) != pdPASS) {
    return false;
  }
  return true;
}
