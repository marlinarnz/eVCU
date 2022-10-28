#ifndef DEVICESPI_H
#define DEVICESPI_H

#include <Arduino.h>
#include <driver/spi_master.h>
#include <driver/timer.h>
#include "DeviceSerial.h"


/** Config data for the serial bus.
 *  Assigns pins, speeds etc. A functional default exists.
 *  @param host         SPI hosts on the ESP32: SPI2_HOST(=1), SPI3_HOST(=2)
 *  @param pinMOSI      MOSI pin
 *  @param pinMISO      MISO pin
 *  @param pinSCLK      CLK pin
 *  @param pinQUADWP    QUADWP pin (optional, set to -1 if not applied)
 *  @param pinQUADHD    QUADHD pin (optional, set to -1 if not applied)
 *  @param pinCS1       CS pin for slave device
 *  @param setDMA       Enable or disable DMA, see ESP32 `spi_bus_initialize()`
 *  @param speed_hz     Bus speed in Hz: 80MHz on the dedicated SPI pins and 40MHz on GPIO-matrix-routed pins
 *  @param dutyCycle    Duty cycle in nth of 256: 128 is 50%
 */
struct configSPI_t {
  int8_t host=1;        // refers to SPI2_HOST but no idea where it is defined
  uint8_t pinMOSI=12;
  uint8_t pinMISO=13;
  uint8_t pinSCLK=15;
  int8_t pinQUADWP=-1;
  int8_t pinQUADHD=-1;
  int8_t pinCS1=14;
  int8_t setDMA=3;      // refers to SPI_DMA_CH_AUTO
  uint32_t speed_hz=5000000;
  uint16_t dutyCycle=128;
};


/** Full description of an SPI transaction for `sendTransaction()`.
 *  Includes the slave and the ESP32 SPI driver transaction description
 *  as pointers respectively.
 */
struct transactionDescr_t {
  spi_transaction_t* desc;
  spi_device_handle_t slave;
  QueueHandle_t queueHandle;
  uint16_t interval;
  uint8_t id;
};


/** Base class for Devices that handle an SPI bus.
 *  Provides functions for initialising and transmissing data on
 *  the bus based on the DeviceSerial class. DeviceSPI instances
 *  must call `startTasks()` and `initSerialProtocol()` in their
 *  `begin()` function and call `endSerialProtocol()` in
 *  their `shutdown()`. They can communicate with one (in theory 3)
 *  receivers/slaves on one SPI bus using `setTransactionPeriodic()`.
 *  Two different transactions can be sent periodically using the
 *  ESP32's hardware timers.
 */
class DeviceSPI : public DeviceSerial
{
public:
  DeviceSPI(VehicleController* vc);
  static bool sendTransaction(transactionDescr_t* trans);
  QueueHandle_t m_queueHandleSendTransaction;

private:
  virtual void onSerialEvent(void* recvBuf, uint8_t len, uint8_t transId);
  void waitForSerialEvent();
  static bool IRAM_ATTR sendTransactionISR(void* trans);
  static void sendTransactionLoop(void* _this);
  TaskHandle_t m_taskHandleSendTransaction;
  static spi_device_interface_config_t getSlaveConfig(int pin, uint32_t speed, uint16_t duty);
  void deleteTrans(transactionDescr_t* trans);

protected:
  bool initSerialProtocol(configSPI_t config);
  void endSerialProtocol();
  void setTransactionPeriodic(uint16_t interval, void* dataBuf, uint8_t len, uint8_t transId);
  virtual void startTasks(uint16_t stackSizeOnValueChanged=4096,
                          uint16_t stackSizeOnSerialEvent=4096);
  spi_device_handle_t m_handleSlave1;
  uint8_t m_host;
  transactionDescr_t* m_pTransOnce;
  transactionDescr_t* m_pTransAtTimer1;
  transactionDescr_t* m_pTransAtTimer2;
};

#endif
