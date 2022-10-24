#include "DeviceCAN.h"


MapTimerMsg* MapTimerMsg::obj{NULL};

/** Returns the instance pointer of the timer-message map class.
 *  Instanciates an object at the first call (Singleton)
 */
MapTimerMsg* MapTimerMsg::getInstance()
{
  if (!obj) obj = new MapTimerMsg();
  return obj;
}


/** Constructor calls the parent constructor.
 */
DeviceCAN::DeviceCAN(VehicleController* vc)
  : DeviceSerial(vc), m_pMap(NULL)
{
  m_pMap = MapTimerMsg::getInstance();
}


/** Waits for incoming messages and notifies the corresponding function.
 *  Regular data frames are forwarded to `onMsgRcv()`, remote frames
 *  go to `onRemoteFrameRcv()`.
 */
void DeviceCAN::waitForSerialEvent()
{
  // Wait for a message
  twai_message_t msg;
  switch (twai_receive(&msg, pdMS_TO_TICKS(1005))) {
    case ESP_OK:
      // Check if the message is remote or normal and forward
      if (msg.rtr) this->onRemoteFrameRcv(&msg);
      else this->onMsgRcv(&msg);
    case ESP_ERR_TIMEOUT:
      // Check for driver errors
      DeviceCAN::checkBusErrors();
    case ESP_ERR_INVALID_STATE:
      PRINT("Error receiving a CAN message: CAN driver is not installed")
      vTaskDelay(pdMS_TO_TICKS(5));
    default:
      PRINT("Error receiving a CAN message")
      break;
  }
}


/** Interface to handle incoming messages.
 *  To be defined in derived class.
 *  @param pMsg: pointer to a twai_message_t message
 */
void DeviceCAN::onMsgRcv(twai_message_t* pMsg)
{}


/** Interface to handle incoming request / remote frames.
 *  To be defined in derived class.
 *  @param pMsg: pointer to a twai_message_t message
 */
void DeviceCAN::onRemoteFrameRcv(twai_message_t* pMsg)
{}


/** Set a message for transaction periodically or once.
 *  Sends a message on the CAN bus immediately, if an interval of 0
 *  is given. Otherwise, it creates a new FreeRTOS software timer
 *  which triggers sending this message periodically. Timer-
 *  message pairs are then saved into the corresponding map. Once
 *  set, the transaction cannot be deleted until all transactions
 *  are stopped and cleared with `endSerialProtocol()`.
 *  @param pMsg pointer to an ESP32 TWAI driver message. Must be
 *              on the heap
 *  @param interval interval between message transmission in ms
 *  @return bool if the message was transmitted successfully in
 *               case the interval was 0, or if the timer was set
 *               up successfully otherwise
 */
bool DeviceCAN::setTransactionPeriodic(twai_message_t* pMsg, uint16_t interval)
{
  // If the interval is 0, send the message once
  if (interval == 0) {
    return DeviceCAN::sendTransaction(pMsg);
  }
  else {
    // Create a FreeRTOS software timer for this message.
    // Its ID is the message identifier
    TimerHandle_t xTimer = xTimerCreate("",
                                        pdMS_TO_TICKS(interval),
                                        pdTRUE,
                                        (void*)(pMsg->identifier),
                                        DeviceCAN::timerCallbackSendTransaction);
    if (xTimer != NULL) {
      // Save the message and its timer in the map with the timer
      // handle as key
      this->m_pMap->put(xTimer, pMsg);
      // Start the timer
      if (xTimerStart(xTimer, 1) != pdPASS) {
        PRINT("Error starting the timer for CAN message " + String(pMsg->identifier))
        return false;
      }
      return true;
    }
    else {
      PRINT("Error creating a timer for CAN message " + String(pMsg->identifier))
      return false;
    }
  }
}


/** Transmit a message on the CAN bus.
 *  Uses the ESP32 TWAI API to send messages.
 *  @param pMsg pointer to an ESP32 TWAI driver message
 *  @return boolean if the transmission was succesful
 */
bool DeviceCAN::sendTransaction(twai_message_t* pMsg)
{
  // Queue the message for transmission
  switch (twai_transmit(pMsg, pdMS_TO_TICKS(5))) {
    case ESP_OK:
      return true;
    case ESP_ERR_TIMEOUT:
      // Check the bus
      DeviceCAN::checkBusErrors();
    case ESP_ERR_INVALID_STATE:
      // Try to start the protocol and send again
      PRINT("Error sending CAN message: Try to start the CAN driver")
      if (twai_start() == ESP_OK) DeviceCAN::sendTransaction(pMsg);
      else DeviceCAN::checkBusErrors();
    case ESP_ERR_NOT_SUPPORTED:
      PRINT("Error sending CAN message because in listen only mode")
    default:
      PRINT("Error sending CAN message because of wrong parameters or send queue disabled")
      break;
  }
  return false;
}


/** Callback function for the send timer.
 *  Calls `sendTransaction` with the message pointer saved in
 *  the map for the given timer handle.
 *  @param xTimer handle of the calling FreeRTOS timer
 */
void DeviceCAN::timerCallbackSendTransaction(TimerHandle_t xTimer)
{
  MapTimerMsg* map = MapTimerMsg::getInstance(); // get Singleton instance
  DeviceCAN::sendTransaction(map->get(xTimer));
}


/** Install the drivers and start serial communication.
 *  This function must be called in the object's `begin()` call.
 *  By default, the TWAI driver ISR is not placed in IRAM.
 *  @param config struct with all information for serial initialisation
 *  @return boolean if the initialisation was successful or not
 */
bool DeviceCAN::initSerialProtocol(configCAN_t config)
{
  // Initialise configuration structures using macro initialisers
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
    config.txPin,
    config.rxPin,
    config.mode);
  twai_timing_config_t t_config;
  if (config.speed == 125000) t_config = TWAI_TIMING_CONFIG_125KBITS();
  else if (config.speed == 250000) t_config = TWAI_TIMING_CONFIG_250KBITS();
  else if (config.speed == 500000) t_config = TWAI_TIMING_CONFIG_500KBITS();
  else if (config.speed == 1000000) t_config = TWAI_TIMING_CONFIG_1MBITS();
  else {
    PRINT("Error: Wrong CAN bus speed given in initialisation")
    return false;
  }
  // Do not set a receive filter because one device cannot know
  // the desired message IDs of other devices.
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  // Install TWAI driver
  // If it was already installed by another device, still return true
  switch (twai_driver_install(&g_config, &t_config, &f_config)) {

    // Success
    case ESP_OK:
      PRINT("Info: CAN driver installed")
      
      // Start TWAI driver
      if (twai_start() == ESP_OK) {
          PRINT("Info: CAN driver started")
          return true;
      } else {
          PRINT("Error starting CAN driver: not in stopped state")
      }

    // Cases where the initialisation went wrong: log an error
    case ESP_ERR_INVALID_STATE:
      PRINT("Debug: CAN driver is already installed")
      return true;
    case ESP_ERR_INVALID_ARG:
      PRINT("Error: Wrong arguments for initialising the CAN driver")
    case ESP_ERR_NO_MEM:
      PRINT("Error initialising the CAN bus: out of memory")
    default:
      PRINT("Error initialising the CAN bus")
      break;
  }
  return false;
}


/** Uninstall the TWAI driver and clear the timer map.
 *  This also deletes the messages from the heap.
 */
void DeviceCAN::endSerialProtocol()
{
  // Delete all timers in the map
  SecuredLinkedListMapElement<TimerHandle_t, twai_message_t*> elementsList[this->m_pMap->size()];
  this->m_pMap->getAll(elementsList);
  for (int i=0; i<this->m_pMap->size(); i++) {
    if (elementsList[i].key != NULL) {
      xTimerDelete(elementsList[i].key, pdMS_TO_TICKS(10));
    }
  }
  // Clear the map
  this->m_pMap->clear();

  // Check the driver status
  twai_status_info_t status;
  if (twai_get_status_info(&status) == ESP_OK) {
    
    // The driver must be in stopped or bus-off state to be uninstalled
    switch (status.state) {
      case TWAI_STATE_STOPPED:
        break;
      case TWAI_STATE_RUNNING:
        // Stop the driver
        twai_stop();
        break;
      case TWAI_STATE_BUS_OFF:
        break;
      case TWAI_STATE_RECOVERING:
        // Wait until it is recovered
        PRINT("Info: Waiting for CAN bus to recover before deinitialisation")
        while (status.state == TWAI_STATE_RECOVERING) {
          vTaskDelay(1);
          twai_get_status_info(&status);
        }
        if (status.state != TWAI_STATE_STOPPED) {
          this->endSerialProtocol();
        }
      default:
        break;
    }

    // Uninstall the driver
    twai_driver_uninstall();
  }
  else {
    PRINT("Error reading status information of CAN driver")
  }
}


/** Checks the CAN bus state.
 *  This helper function reports warnings and tries to recover the
 *  bus to running state.
 */
void DeviceCAN::checkBusErrors()
{
  // Check the driver status
  twai_status_info_t status;
  if (twai_get_status_info(&status) == ESP_OK) {
    switch (status.state) {
      case TWAI_STATE_STOPPED:
        if (twai_start() != ESP_OK) {
          PRINT("Error starting CAN bus: Driver not installed")
          return;
        }
      case TWAI_STATE_RUNNING:
        break;
      case TWAI_STATE_BUS_OFF:
        // Try to recover the bus
        if (twai_initiate_recovery() == ESP_OK) {
          PRINT("Info: Started CAN bus recovery")
          vTaskDelay(pdMS_TO_TICKS(50));
        }
        else {
          PRINT("Error recovering the CAN bus")
          return;
        }
      case TWAI_STATE_RECOVERING:
        // Wait until it is recovered
        PRINT("Info: Waiting for CAN bus to recover")
        while (status.state == TWAI_STATE_RECOVERING) {
          vTaskDelay(1);
          twai_get_status_info(&status);
        }
        twai_start();
        PRINT("Info: CAN bus recovered and started")
      default:
        break;
    }
  }
  else {
    PRINT("Error reading status information of CAN driver")
  }

  /*// Check TWAI driver alerts
  uint32_t alerts;
  switch (twai_read_alerts(&alerts, 1) {
    case ESP_OK:
      // Read alerts
      if ()
    case ESP_ERR_TIMEOUT:
      PRINT("Debug: No CAN bus alert occured")
    default:
      PRINT("Error reading CAN bus alerts")
  }*/
}


/** Starts tasks from `DeviceSerial`.
 *  Must be called in the derived class's `begin()` function.
 *  @param stackSizeOnValueChanged size of onValueChangedLoop task
 *         stack in bytes. Default is 4096
 *  @param stackSizeOnPinInterrupt size of onPinInterruptLoop task
 *         stack in bytes. Default is 4096
 */
void DeviceCAN::startTasks(uint16_t stackSizeOnValueChanged,
                           uint16_t stackSizeOnSerialEvent)
{
  // Call parent function to start the rest of the tasks
  DeviceSerial::startTasks(stackSizeOnValueChanged, stackSizeOnSerialEvent);
}
