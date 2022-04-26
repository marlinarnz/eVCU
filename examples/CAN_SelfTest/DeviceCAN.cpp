#include "DeviceSPI.h"


/** Constructor calls the parent constructor.
 */
DeviceSPI::DeviceSPI(VehicleController* vc)
  : DeviceSerial(vc), m_queueHandleSendTransaction(NULL), m_taskHandleSendTransaction(NULL)
{}


/** Waits for transaction results and notifies `onSerialEvent()`.
 *  First, it checks results on one-time transactions, then for timer
 *  one and then for timer two without getting blocked through wait
 *  times.
 */
void DeviceSPI::waitForSerialEvent()
{
  // Check the one-time transactions
  if (this->m_pTransOnce != NULL) {
    transactionDescr_t* pTransDescr = this->m_pTransOnce;
    // Check for transaction results
    switch (spi_device_get_trans_result(this->m_handleSlave1,
                                        &(pTransDescr->desc), 0))
    {
      case ESP_OK:
        // Set the one-time transaction pointer back to NULL
        this->m_pTransOnce = NULL;
        // Forward the result
        this->onSerialEvent(pTransDescr->desc->rx_buffer,
                            pTransDescr->desc->length/8,
                            pTransDescr->id);
        // Delete one-time transaction after receiving it
        DeviceSPI::deleteTrans(pTransDescr);
        break;
      case ESP_ERR_TIMEOUT:
        break;
      default:
        PRINT("Error receiving one-time SPI transaction")
        this->m_pTransOnce = NULL;
        DeviceSPI::deleteTrans(pTransDescr);
        break;
    }
  }

  // Fetch the result of the last timed transaction
  if (this->m_pTransAtTimer1 != NULL) {
    switch(spi_device_get_trans_result(this->m_handleSlave1,
                                       &(this->m_pTransAtTimer1->desc), 0))
    {
      case ESP_OK:
        // Forward the transaction results to the logic function
        this->onSerialEvent(this->m_pTransAtTimer1->desc->rx_buffer,
                            this->m_pTransAtTimer1->desc->length/8,
                            this->m_pTransAtTimer1->id);
        break;
      case ESP_ERR_TIMEOUT:
        break;
      default:
        PRINT("Error receiving results from an SPI transaction on timer one")
        break;
    }

    // If there was one on timer 1, timer 2 might also have results
    if (this->m_pTransAtTimer2 != NULL) { 
      switch(spi_device_get_trans_result(this->m_handleSlave1,
                                         &(this->m_pTransAtTimer2->desc), 0))
      {
        case ESP_OK:
          // Forward the transaction results to the logic function
          this->onSerialEvent(this->m_pTransAtTimer2->desc->rx_buffer,
                              this->m_pTransAtTimer2->desc->length/8,
                              this->m_pTransAtTimer2->id);
          break;
        case ESP_ERR_TIMEOUT:
          break;
        default:
          PRINT("Error receiving results from an SPI transaction on timer two")
          break;
      }
    }
  }
}


/** Interface to handle the results of SPI transactions.
 *  To be defined in the derived class.
 *  @param recvBuf data buffer with message from the slave device
 *  @param len length of the receive data buffer
 *  @param transId transaction ID, if set in call to `setTransactionPeriodic()`
 */
void DeviceSPI::onSerialEvent(void* recvBuf, uint8_t len, uint8_t transId)
{}


/** Invokes a SPI transaction periodically or only once.
 *  SPI master transmissions send and receive in one command, but the
 *  ESP32 driver allows asynchronous sending and receiving. The
 *  latter is coordinated by `waitForSerialEvent()`. This function creates
 *  transmissions and manages timers to send them. The ESP32 allows setting
 *  two timers. The fist periodic transmission must not have a longer
 *  interval than the second and the second must not have more than double
 *  of the first.
 *  @param interval wait time before repeating the transaction in millis. `0`
 *                  will execute the transaction just once
 *  @param dataBuf pointer-type buffer for the data to write
 *  @param len length of the `dataBuf` and the maximum length of the receive
 *             buffer in bytes
 *  @param transId optional parameter to give this transaction a unique
 *                 identifyer. Useful to interpret periodic return values
 */
void DeviceSPI::setTransactionPeriodic(uint16_t interval, void* dataBuf, uint8_t len, uint8_t transId)
{
  // Init the transaction: create it on heap
  spi_transaction_t* trans = new spi_transaction_t;
  memset(trans, 0, sizeof(*trans));
  trans->length = len * 8;
  trans->tx_buffer = dataBuf;
  byte recvBuf[len] = {0};
  trans->rx_buffer = recvBuf;
  
  transactionDescr_t* phTransDesc = new transactionDescr_t;
  phTransDesc->desc=trans;
  phTransDesc->slave=this->m_handleSlave1;
  phTransDesc->queueHandle=this->m_queueHandleSendTransaction;
  phTransDesc->interval=interval;
  phTransDesc->id=transId;

  if (interval == 0) {
    if (this->m_pTransOnce == NULL) {
      // Send transaction once
      this->m_pTransOnce = phTransDesc;
      if (!DeviceSPI::sendTransaction(phTransDesc)) {
        PRINT("Error sending SPI transaction. Transaction queue full or wrong parameters")
      }
    } else {
      PRINT("Error sending SPI transaction: another one-time transaction is waiting to receive results")
    }
    
  } else {
    if (this->m_pTransAtTimer1 == NULL || this->m_pTransAtTimer2 == NULL) {
      // Initialise the timer
      timer_group_t group = (this->m_host==1) ? TIMER_GROUP_0 : TIMER_GROUP_1;
      uint8_t id = (this->m_pTransAtTimer1 == NULL) ? 0 : 1;
      const timer_config_t config = {
        .alarm_en=TIMER_ALARM_EN,
        .counter_en=TIMER_PAUSE,
        .intr_type=TIMER_INTR_LEVEL,
        .counter_dir=TIMER_COUNT_UP,
        .auto_reload=TIMER_AUTORELOAD_EN,
        .divider=80 // Frequency is 80MHz
      };
      if (ESP_OK == timer_init(group, (timer_idx_t)id, &config)) {

        // Set the timer
        // 1MHz / (1000*interval[ms]) is 
        timer_set_alarm_value(group, (timer_idx_t)id, 1000 * interval);
        // Add the ISR
        timer_enable_intr(group, (timer_idx_t)id);
        if (ESP_OK == timer_isr_callback_add(group, (timer_idx_t)id,
                                             sendTransactionISR, phTransDesc, 0))
        {
          if (id == 0) this->m_pTransAtTimer1 = phTransDesc;
          else this->m_pTransAtTimer2 = phTransDesc;
          timer_start(group, (timer_idx_t)id);
          PRINT("Info: Set up SPI transaction timer successfully")
        } else {
          PRINT("Error: Failed to add the ISR to the SPI transaction timer")
          DeviceSPI::deleteTrans(phTransDesc);
        }
      } else {
        PRINT("Error: Wrong SPI transaction timer initialisation config parameter(s)")
        DeviceSPI::deleteTrans(phTransDesc);
      }
    } else {
      PRINT("Error: maximum number of SPI transaction timers already set (2). Cannot set more")
      DeviceSPI::deleteTrans(phTransDesc);
    }
  }
}


/** Sends SPI transactions on the queue.
 *  Different ESP32 hardware timers can use this function in order to
 *  send their transactions on the queue using `spi_device_queue_trans()`.
 *  @param trans description of the SPI transaction of `transactionDescr_t`
 *  @return boolean if the transaction was queued successfully
 */
bool DeviceSPI::sendTransaction(transactionDescr_t* trans)
{
  if (ESP_OK == spi_device_queue_trans(trans->slave, trans->desc, 0)) {
    return true;
  } else {
    // Queue is full
    return false;
  }
}


/** ISR to initiate sending an SPI transaction.
 *  Puts the transaction in an event queue that is observed by a higher-
 *  priority task, which just calls `sendTransaction()`. This workaround
 *  is necessary because `spi_device_queue_trans()` cannot be called from
 *  ISRs as it uses the FreeRTOS API function `xQueueSend()`.
 *  @param trans description of the SPI transaction of `transactionDescr_t`
 *  @return boolean if the ISR should yield to a higher priority task
 */
bool IRAM_ATTR DeviceSPI::sendTransactionISR(void* trans)
{
  BaseType_t highTaskAwoken = pdFALSE;
  transactionDescr_t* info = (transactionDescr_t*) trans;
  if (info != NULL) xQueueSendFromISR(info->queueHandle, &trans, &highTaskAwoken);
  return highTaskAwoken == pdTRUE; // return whether to yield at the end of ISR
}


/** Task function for sending SPI transactions from ISRs.
 *  @see `sendTransactionISR()`
 *  @param _this pointer to the current object of void* type
 */
void DeviceSPI::sendTransactionLoop(void* _this)
{
  // Cast this pointer to get queue handle
  QueueHandle_t queueHandle = static_cast<DeviceSPI*>(_this)->m_queueHandleSendTransaction;
  for (;;) {
    // Wait for a transaction to send
    transactionDescr_t* trans = NULL;
    xQueueReceive(queueHandle, &(trans), portMAX_DELAY);
    DeviceSPI::sendTransaction(trans);
  }
  vTaskDelete(NULL);
}


/** Install the drivers and start serial communication.
 *  This function must be called in the object's `begin()` call.
 *  @param config struct with all information for serial initialisation
 *  @return boolean if the initialisation was successful or not
 */
bool DeviceSPI::initSerialProtocol(configSPI_t config)
{
  // Set this object's SPI-relevant members
  this->m_host = config.host;
  this->m_pTransOnce = NULL;
  this->m_pTransAtTimer1 = NULL;
  this->m_pTransAtTimer2 = NULL;
  
  // Prepare a struct with pin information
  static spi_bus_config_t spiPins={
    .mosi_io_num=config.pinMOSI,
    .miso_io_num=config.pinMISO,
    .sclk_io_num=config.pinSCLK,
    .quadwp_io_num=config.pinQUADWP,
    .quadhd_io_num=config.pinQUADHD
  };

  // Init the driver
  switch(spi_bus_initialize((spi_host_device_t)config.host, &spiPins, config.setDMA)) {
    
    case ESP_OK:
      // All good, so we can continue
      // Add slaves, if their pins are defined
      PRINT("Info: started SPI bus")
      esp_err_t returnVal;

      // Try to add slave 1
      if (config.pinCS1 > -1) {
        static spi_device_interface_config_t slaveConfig = DeviceSPI::getSlaveConfig(
          config.pinCS1, config.speed_hz, config.dutyCycle);
        returnVal = spi_bus_add_device((spi_host_device_t)config.host, &slaveConfig, &(this->m_handleSlave1));
        if (returnVal == ESP_ERR_INVALID_ARG) PRINT("Error adding SPI slave 1: invalid argumend in config.")
        else if (returnVal == ESP_ERR_NOT_FOUND) PRINT("Error adding SPI slave 1: no CS slot available.")
        else if (returnVal == ESP_ERR_NO_MEM) PRINT("Error adding SPI slave 1: out of memory.")
        else if (returnVal == ESP_OK) {
          PRINT("Info: added receiver device to SPI bus")
          return true;
        }
      } else {
        PRINT("Warning: no slave defined for SPI communication")
        return true;
      }

    // Cases where the initialisation went wrong return false and log an error
    case ESP_ERR_INVALID_STATE:
      PRINT("Error initialising the SPI bus: host is already in use")
    case ESP_ERR_NOT_FOUND:
      PRINT("Error initialising the SPI bus: no DMA channel available, but demanded in configuration file")
    case ESP_ERR_NO_MEM:
      PRINT("Error initialising the SPI bus: out of memory")
    default:
      PRINT("Error initialising the SPI bus. Check the pins.")
      break;
  }
  return false;
}


/** Clear the bus and free the RAM
 */
void DeviceSPI::endSerialProtocol()
{
  // End timers
  timer_group_t group = (this->m_host==1) ? TIMER_GROUP_0 : TIMER_GROUP_1;
  if (this->m_pTransAtTimer1 != NULL) timer_deinit(group, (timer_idx_t)0);
  if (this->m_pTransAtTimer2 != NULL) timer_deinit(group, (timer_idx_t)1);

  // delete transactions
  if (this->m_pTransAtTimer1 != NULL) DeviceSPI::deleteTrans(this->m_pTransAtTimer1);
  if (this->m_pTransAtTimer2 != NULL) DeviceSPI::deleteTrans(this->m_pTransAtTimer2);

  // Unregister slaves
  spi_bus_remove_device(this->m_handleSlave1);
}


/**
 * Helper function to get a driver-compatible slave config file.
 * @param pin GPIO corresponding to the number of the slave (1, 2 or 3)
 * @param speed speed of the transmission in Hz
 * @param duty duty cycle of the transmission
 * @return spi_device_interface_config_t config struct
 */
spi_device_interface_config_t DeviceSPI::getSlaveConfig(int pin, uint32_t speed, uint16_t duty)
{
  // Prepare the struct with receiving device settings
  static spi_device_interface_config_t receiveConfig={
    .command_bits=0,
    .address_bits=0,
    .dummy_bits=0,
    .mode=0,
    .duty_cycle_pos=duty,
    .cs_ena_pretrans=0,
    .cs_ena_posttrans=3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
    .clock_speed_hz=speed,
    .input_delay_ns=0,
    .spics_io_num=pin,
    .flags=0,
    .queue_size=2,              //Queue size for transactions to be sent
    .pre_cb=NULL,               //Callback function prior and after transmission
    .post_cb=NULL
  };
  return receiveConfig;
}


/** Helper function to delete outdated transactions
 */
void DeviceSPI::deleteTrans(transactionDescr_t* trans)
{
  delete trans->desc;
  delete trans;
}


/** Starts tasks from `DeviceSerial` and the `sendTransactionLoop()`.
 *  Must be called in the derived class's `begin()` function.
 *  @param stackSizeOnValueChanged size of onValueChangedLoop task
 *         stack in bytes. Default is 4096
 *  @param stackSizeOnPinInterrupt size of onPinInterruptLoop task
 *         stack in bytes. Default is 4096
 */
void DeviceSPI::startTasks(uint16_t stackSizeOnValueChanged,
                           uint16_t stackSizeOnSerialEvent)
{
  // Create the transaction queue
  this->m_queueHandleSendTransaction = xQueueCreate(2, sizeof(transactionDescr_t*));
  
  // Start the send task
  xTaskCreatePinnedToCore(
    this->sendTransactionLoop, // function name
    "sendTransaction", // Name for debugging
    (uint16_t)(4096/4),
    this, // Parameters pointer for the function; must be static
    3, // Priority (1 is lowest)
    &m_taskHandleSendTransaction, // task handle
    1 // CPU core
  );
  if (m_taskHandleSendTransaction == NULL) {
    PRINT("Fatal: failed to create task sendTransactionLoop")
  }

  // Call parent function to start the rest of the tasks
  DeviceSerial::startTasks(stackSizeOnValueChanged, stackSizeOnSerialEvent);
}
