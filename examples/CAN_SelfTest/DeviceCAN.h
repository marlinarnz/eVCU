#ifndef DEVICECAN_H
#define DEVICECAN_H

#include <Arduino.h>
#include <driver/twai.h>
#include <driver/timer.h>
#include "DeviceSerial.h"


/** Config data for the serial bus.
 *  Assigns pins, speeds etc. A functional default exists.
 *  @param host         S
 */
struct configCAN_t {
  int8_t host=1;
};


/** Base class for Devices that handle the CAN bus.
 *  Provides functions for initialising and transmissing data on
 *  the bus based on the DeviceSerial class. DeviceCAN instances
 *  must call `startTasks()` and `initSerialProtocol()` in their
 *  `begin()` function and call `endSerialProtocol()` in
 *  their `shutdown()`. They can send messages on the bus using
 *  `setTransactionPeriodic()`. Two different transactions can
 *  be sent periodically using the ESP32's hardware timers.
 */
class DeviceCAN : public DeviceSerial
{
public:
  DeviceCAN(VehicleController* vc);

private:
  virtual void onSerialEvent(void* recvBuf, uint8_t len, uint8_t transId);
  void waitForSerialEvent();
  void onSerialEventLoop(void* pvParameters);
  static bool IRAM_ATTR sendTransactionISR(void* trans);
  static void sendTransactionLoop(void* _this);
  TaskHandle_t m_taskHandleSendTransaction;
  void deleteTrans(transactionDescr_t* trans);

protected:
  static bool sendTransaction(transactionDescr_t* trans);
  QueueHandle_t m_queueHandleSendTransaction;
  bool initSerialProtocol(configCAN_t config);
  void endSerialProtocol();
  void setTransactionPeriodic(uint16_t interval, void* dataBuf, uint8_t len, uint8_t transId);
  static void startOnSerialEventLoop(void* _this);
  virtual void startTasks(uint16_t stackSizeOnValueChanged=4096,
                          uint16_t stackSizeOnSerialEvent=4096);
  TaskHandle_t m_taskHandleOnSerialEvent;
  transactionDescr_t* m_pTransOnce;
  transactionDescr_t* m_pTransAtTimer1;
  transactionDescr_t* m_pTransAtTimer2;
};

#endif
