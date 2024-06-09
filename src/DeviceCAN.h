#ifndef DEVICECAN_H
#define DEVICECAN_H

#include <Arduino.h>
#include <driver/twai.h>
#include "DeviceSerial.h"
#include "SecuredLinkedListMap.h"


/** Config data for the serial bus.
 *  Assigns pins, speeds etc. A functional default exists.
 *  @param txPin        TX pin to connect to the CAN transceiver as `gpio_num_t`
 *  @param rxPin        RX pin to connect to the CAN transceiver as `gpio_num_t`
 *  @param mode         TWAI driver mode: `TWAI_MODE_NORMAL`,
 *                      `TWAI_MODE_NO_ACK` or `TWAI_MODE_LISTEN_ONLY`
 *  @param speed        bus speed 125, 250, 500 or 1000 kbps
 */
struct configCAN_t {
  gpio_num_t txPin=GPIO_NUM_21;
  gpio_num_t rxPin=GPIO_NUM_22;
  twai_mode_t mode=TWAI_MODE_NORMAL;
  uint32_t speed=500000;
};


/** Message data struct
 *  Stores message pointer and the message's counter specifics
 *  @param pMsg       pointer to the ESP32 TWAI message
 *  @param pCountByte pointer to the message's counter byte
 *  @param countLSB   least significant bit of the counter
 *  @param countLen   length (bits) of the counter
 *  @param checkByte  byte number of message's checksum
 */
struct msgData_t {
  twai_message_t* pMsg=NULL;
  uint8_t* pCountByte=NULL;
  uint8_t countLSB=0;
  uint8_t countLen=0;
  uint8_t checkByte=255;
};


/** Map class that connects timers with corresponding messages.
 *  Keys are FreeRTOS timer handles and values are message data struct
 *  pointers. The map allows a callback function to determine which
 *  message should be sent when a timer alerts. It's a singleton class.
 */
class MapTimerMsg : public SecuredLinkedListMap<TimerHandle_t, msgData_t*>
{
private:
  static MapTimerMsg* obj;
  MapTimerMsg() {}  //* Private constructor
  ~MapTimerMsg() {}
public:
  static MapTimerMsg* getInstance();  //* Returns instance pointer
};


/** Base class for Devices that interact with the CAN bus.
 *  Provides functions for initialising and transmitting data on
 *  the bus based on the DeviceSerial class. DeviceCAN instances
 *  must call `startTasks()` in their `begin()` function. At least
 *  one `DeviceCAN` instance must call `initSerialProtocol()` in
 *  its `begin()` function and `endSerialProtocol()` in its
 *  `shutdown()`. As there is only one CAN bus peripheral and driver
 *  on the ESP32, it makes sense to have only one `DeviceCAN`
 *  instance, a CAN manager, which handles reading and writing
 *  messages.
 */
class DeviceCAN : public DeviceSerial
{
public:
  DeviceCAN(VehicleController* vc);

private:
  virtual void onMsgRcv(twai_message_t* pMsg);
  virtual void onRemoteFrameRcv(twai_message_t* pMsg);
  void waitForSerialEvent();
  static bool sendTransaction(twai_message_t* pMsg);
  static void incrementCounter(msgData_t* pMsgData);
  static void updateChecksum(msgData_t* pMsgData);
  static void timerCallbackSendTransaction(TimerHandle_t xTimer);
  static void checkBusErrors();
  twai_mode_t m_busMode;

protected:
  MapTimerMsg* m_pMap;
  bool setTransactionPeriodic(twai_message_t* pMsg,
                              uint16_t interval,
                              uint8_t* pCounterByte=NULL,
                              uint8_t counterLSB=0,
                              uint8_t counterLen=0,
                              uint8_t checksumByte=255);
  bool initSerialProtocol(configCAN_t config);
  void endSerialProtocol();
  virtual void startTasks(uint16_t stackSizeOnValueChanged=4096,
                          uint16_t stackSizeOnSerialEvent=4096,
                          uint8_t core=1);
};

#endif
