#include "DeviceSPI.h"


/** Waits for transaction results and notifies `onSerialEvent()`.
 *  First, it checks results on one-time transactions, then for timer
 *  one and then for timer two without getting blocked through wait
 *  times.
 */
void DeviceSPI::waitForSerialEvent()
{
  // Check the one-time transactions
  if (this->m_pTransOnce != NULL) {
    if (ESP_OK == spi_device_get_trans_result(this->m_handleSlave1,
                                              &(this->m_pTransOnce->desc), 0))
    {
      // Forward the result
      this->onSerialEvent(this->m_pTransOnce->desc->rx_buffer,
                          *(this->m_pTransOnce->desc->user));
    }
    // Delete one-time transaction
    DeviceSPI::deleteTrans(this->m_pTransOnce->desc);
    delete this->m_pTransOnce;
  }

  // Fetch the result of the last queued transaction
  if (this->m_pTransAtTimer1 != NULL) {  
    switch(spi_device_get_trans_result(this->m_handleSlave1,
                                       &(this->m_pTransAtTimer1->desc), 0))
    {
      case ESP_OK:
        // Forward the transaction results to the logic function
        this->onSerialEvent(this->m_pTransAtTimer1->desc->rx_buffer,
                            *(this->m_pTransAtTimer1->desc->user));
        break;
      case ESP_ERR_TIMEOUT:
        //PRINT("Debug: waiting for SPI transaction results on timer one timed out")
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
                              *(this->m_pTransAtTimer2->desc->user));
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
 *  @param transId transaction ID, if set in call to `setTransactionPeriodic()`
 */
void DeviceSPI::onSerialEvent(void* recvBuf, uint8_t transId)
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
  byte* recvBuf = new byte[len];
  trans->rx_buffer = recvBuf;
  trans->user = &transId;
  
  transactionDescr_t transDesc = {
    .desc=trans,
    .slave=this->m_handleSlave1,
    .interval=interval,
    .delAfterRead= (interval==0) ? true : false
  };

  if (interval == 0) {
    // Send transaction once
    if (!DeviceSPI::sendTransaction(&transDesc)) {
      PRINT("Error sending SPI transaction. Transaction queue full or wrong parameters")
    }
    this->m_pTransOnce = &transDesc;
    
  } else {
    if (this->m_pTransAtTimer1 == NULL || this->m_pTransAtTimer2 == NULL) {
      // Init the timer
      timer_group_t group = (this->m_host==1) ? TIMER_GROUP_0 : TIMER_GROUP_1;
      uint8_t id = (this->m_pTransAtTimer1 == NULL) ? 0: 1;
      const timer_config_t config = {
        .alarm_en=TIMER_ALARM_EN,
        .counter_en=TIMER_START,
        .intr_type=TIMER_INTR_LEVEL,
        .counter_dir=TIMER_COUNT_UP,
        .auto_reload=TIMER_AUTORELOAD_EN,
        .divider=
      };
      if (ESP_OK == timer_init(group, (timer_idx_t)id, &config)) {
        
        // Add the ISR
        if (ESP_OK == timer_isr_callback_add(group, (timer_idx_t)id,
                                             sendTransaction, &transDesc, 0))
        {
          PRINT("Info: Set up SPI transaction timer successfully")
          if (id == 0) this->m_pTransAtTimer1 = trans;
          else this->m_pTransAtTimer2 = trans;
        } else {
          PRINT("Error: Failed to add the ISR to the SPI transaction timer")
          DeviceSPI::deleteTrans(trans);
        }
      } else {
        PRINT("Error: Wrong SPI transaction timer initialisation config parameter(s)")
        DeviceSPI::deleteTrans(trans);
      }
    } else {
      PRINT("Error: maximum number of SPI transaction timers already set (2). Cannot set more")
      DeviceSPI::deleteTrans(trans);
    }
  }
}


/** Interrupt Service Routine sends SPI transactions on the queue.
 *  Different ESP32 hardware timers can use this ISR in order to
 *  send their transactions on the queue using `spi_device_queue_trans()`.
 *  @param trans description of the SPI transaction of `transactionDescr_t`
 *  @return boolean if the transaction was queued successfully
 */
bool IRAM_ATTR DeviceSPI::sendTransaction(void* trans)
{
  transactionDescr_t* info = (transactionDescr_t*) trans;
  if (ESP_OK == spi_device_queue_trans(info->slave, info->desc, 0)) {
    return true;
  } else {
    return false;
  }
}


/** Install the drivers and start serial communication.
 *  This function must be called in the object's `begin()` call.
 *  @param config struct with all information for serial initialisation.
 *                Defaults to `defaultSPIConfig`
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
  if (this->m_pTransAtTimer1 != NULL) DeviceSPI::deleteTrans(this->m_pTransAtTimer1->desc);
  if (this->m_pTransAtTimer2 != NULL) DeviceSPI::deleteTrans(this->m_pTransAtTimer2->desc);

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
    .queue_size=3,              //Queue size!!
    .pre_cb=NULL,               //Callback function prior and after transmission
    .post_cb=NULL
  };
  return receiveConfig;
}


/** Helper function to delete outdated transactions
 */
void DeviceSPI::deleteTrans(spi_transaction_t* trans)
{
  delete trans->user;
  delete trans->tx_buffer;
  delete trans->rx_buffer;
  delete trans;
}
