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


/** Map class that connects timers with corresponding messages.
 *  Keys are FreeRTOS timer handles and values are ESP32 TWAI message
 *  pointers. The map allows a callback function to determine which
 *  message should be sent when a timer alerts. It's a singleton class.
 */
class MapTimerMsg : public SecuredLinkedListMap<TimerHandle_t, twai_message_t*>
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
  static void timerCallbackSendTransaction(TimerHandle_t xTimer);
  static void checkBusErrors();

protected:
  MapTimerMsg* m_pMap;
  bool setTransactionPeriodic(twai_message_t* pMsg, uint16_t interval);
  bool initSerialProtocol(configCAN_t config);
  void endSerialProtocol();
  virtual void startTasks(uint16_t stackSizeOnValueChanged=4096,
                          uint16_t stackSizeOnSerialEvent=4096);
};

#endif
