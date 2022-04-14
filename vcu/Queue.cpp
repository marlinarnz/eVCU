#include "Queue.h"


/** The constructor instantiates the queue and mutex.
 *  The message size of the queue is 4 bytes, as it saves pointers.
 */
Queue::Queue()
  : m_handleQueue(NULL), m_mutex(AccessControl())
{
  m_handleQueue = xQueueCreate(QUEUE_SIZE, 4);
  if (m_handleQueue==NULL) {
    if (DEBUG) {PRINT("Fatal: Queue generation failed")}
  }
}


/** The destructor deletes the queue.
 */
Queue::~Queue()
{
  vQueueDelete(m_handleQueue);
  m_handleQueue = NULL;
}


/** Checks if the queue contains messages.
 *  @return true if the queue is empty, else false
 */
bool Queue::empty()
{
  AccessLock lock(&m_mutex);
  Parameter* pParam;
  // Don't wait, if the element is not immediately available
  return !xQueuePeek(m_handleQueue, &(pParam), (TickType_t)0);
}


/** Retrieves the first message of the queue and deletes it.
 *  @return pointer to the Parameter
 */
Parameter* Queue::pop()
{
  AccessLock lock(&m_mutex);
  Parameter* pParam = NULL;
  // Don't wait, if the element is not immediately available
  xQueueReceive(m_handleQueue, &(pParam), (TickType_t)0);
  return pParam;
}


/** Pushes a new message to the end of the queue.
 *  It waits a short time, if the queue is not available immediately.
 *  @param pParam pointer to the Parameter that is added
 *  @return boolean if the Parameter was added successfully
 */
bool Queue::push(Parameter* pParam)
{
  AccessLock lock(&m_mutex);
  // Wait 10 millis if the queue is already full
  if (xQueueSend(m_handleQueue, (void*) &pParam, pdMS_TO_TICKS(10)) != pdPASS) {
    return false;
  }
  return true;
}
