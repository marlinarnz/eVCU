/* ============================== Utility functions ======================= 
 * This file defines utility funtions for communication including the
 * CanManager class.
 */

#include "commUtils.h"
#include <EEPROM.h>


/* ============================== Report messages =========================
 * Transmits messages to the UI using a CAN frame
 * @param msg: integer message ID
 * @param level: integer 0 = "debug"; 1 = "info"; 2 = "warn"; 3 = "error"
 * @param warn: (optional) boolean whether to set the VCU warning level
 */
void report(String msg, int level, bool warn) {  //TODO
  if (!Serial.available()) {
    Serial.begin(115200);       // Initialize the USB serial
    while (!Serial) {;}         // Wait until it started. Only for native port
  }
  Serial.println(msg);

  //TODO fill bytes of CAN frame VCUMSG with id of msg (6 bits) and level (2 bits)
  // results in 63 possible messages
  //
  // Report the loop time on debug level
  //String loopTime = String(millis() - lastLoop);
  //report("Loop time (ms): " + loopTime, 0);

  if (warn) {
    setWarningLevel(level);
  }
}


/* ============================== Warning level ===========================
 * Sets the warning level of the VCU
 * @param lvl: integer 0 = "OK"; 1 = "low"; 2 = "medium"; 3 = "high"
 */
void setWarningLevel(int lvl) {
  EEPROM.put(VCU1_WARNING_LEVEL, lvl);
}


/* ============================== EEPROM ==================================
 * The resetEEPROM() function must be run once in the initial setup.
 * Afterwards, it resets all values back to default.
 * NOTE: There is a limit of 100,000 write cycles per single location.
 * Therefore, using put() is preferred as it includes the update() function
 */
void resetEEPROM() {
  EEPROM.put(VCU1_WARNING_LEVEL, vcu1WarningLevel);
  EEPROM.put(COOL_THRESHOLD, coolThreshold);
  EEPROM.put(MAX_TORQUE, maxTorque);
  EEPROM.put(MAX_NEG_TORQUE, maxNegTorque);
  EEPROM.put(MAX_RPM, maxRPM);
  EEPROM.put(BRAKE_LIGHT_TORQUE_THRESHOLD, brakeLightTorqueThreshold);
  EEPROM.put(THROTTLE_POS_MAX_TORQUE, throttlePosMaxTorque);
  EEPROM.put(THROTTLE_POS_HALF_TORQUE, throttlePosHalfTorque);
  EEPROM.put(THROTTLE_POS_ZERO_TORQUE, throttlePosZeroTorque);
  EEPROM.put(THROTTLE_POS_ZERO_NEG_TORQUE, throttlePosZeroNegTorque);
  EEPROM.put(THROTTLE_POS_MAX_NEG_TORQUE, throttlePosMaxNegTorque);
  EEPROM.put(THROTTLE_POS_START, throttlePosStart);
  EEPROM.put(BRAKE_PEDAL_FUNCTION, brakePedalFunction);
}


/* ============================== CAN constructor =========================
 * The constructor saves the given CAN object and instantiates CAN messages
 * for every known ID as own attributes.
 * @param canObj: MCP_CAN class object pointer
 */
CanManager::CanManager(MCP_CAN* canObj) {
  MCP_CAN* _canObj = canObj;
  // Construct the known messages
  CanMessage messagesVCU[N_VCU_MESSAGES] = {
    {VCU1, _canObj, VCU1_INTERVAL},
    {VCU2, _canObj, VCU2_INTERVAL},
    {VCU3, _canObj, VCU3_INTERVAL}
  };
  CanMessage messagesOther[N_OTHER_MESSAGES] = {
    {MCU1, _canObj, MCU1_INTERVAL},
    {MCU2, _canObj, MCU2_INTERVAL},
    {MCU3, _canObj, MCU3_INTERVAL},
    {UI_ThrottlePos, _canObj, UI_ThrottlePos_INTERVAL},
    {UI_DriveSettings, _canObj, UI_DriveSettings_INTERVAL}
  };

  // Fill the VCU messages with initial values (from EEPROM)
  for (uint8_t i=0; i<N_VCU_MESSAGES; i++) {
    switch(messagesVCU[i].getId()) {
      case VCU1:
        messagesVCU[i].writeSignal(VCU1_TorqueReq_LSB, VCU1_TorqueReq_LEN, 0, VCU1_TorqueReq_CONV_D);
        messagesVCU[i].writeSignal(VCU1_MotorSpdReq_LSB, VCU1_MotorSpdReq_LEN, 0, VCU1_MotorSpdReq_CONV_D);
        messagesVCU[i].writeSignal(VCU1_BrakePedalSts_LSB, VCU1_BrakePedalSts_LEN, VCU1_BrakePedalSts_FREE);
        messagesVCU[i].writeSignal(VCU1_BMS_MainRelayCmd_LSB, VCU1_BMS_MainRelayCmd_LEN, VCU1_BMS_MainRelayCmd_OFF);
        messagesVCU[i].writeSignal(VCU1_GearLeverPosSts_LSB, VCU1_GearLeverPosSts_LEN, VCU1_GearLeverPosSts_DEFAULT);
        messagesVCU[i].writeSignal(VCU1_GearLeverPosSts_F_LSB, VCU1_GearLeverPosSts_F_LEN, VCU1_GearLeverPosSts_F_OK);
        messagesVCU[i].writeSignal(VCU1_WorkMode_LSB, VCU1_WorkMode_LEN, VCU1_WorkMode_TORQUE);
        messagesVCU[i].writeSignal(VCU1_MotorMode_LSB, VCU1_MotorMode_LEN, VCU1_MotorMode_STANDBY);
        messagesVCU[i].writeSignal(VCU1_WarningLevel_LSB, VCU1_WarningLevel_LEN, EEPROM.read(VCU1_WARNING_LEVEL));
        messagesVCU[i].writeSignal(VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN, VCU1_KeyPosition_OFF);
        messagesVCU[i].writeSignal(VCU1_RollingCounter_LSB, VCU1_RollingCounter_LEN, 0);
        messagesVCU[i].writeSignal(VCU1_BMS_AuxRelayCmd_LSB, VCU1_BMS_AuxRelayCmd_LEN, VCU1_BMS_AuxRelayCmd_OFF);
        break;
      case VCU2:
        messagesVCU[i].writeSignal(VCU2_ChargeEnable_LSB, VCU2_ChargeEnable_LEN, VCU2_ChargeEnable_OFF);
        messagesVCU[i].writeSignal(VCU2_UrgencyCutOffPowerReq_LSB, VCU2_UrgencyCutOffPowerReq_LEN, VCU2_UrgencyCutOffPowerReq_OK);
        messagesVCU[i].writeSignal(VCU2_BMS_HeatingCmd_LSB, VCU2_BMS_HeatingCmd_LEN, VCU2_BMS_HeatingCmd_OFF);
        messagesVCU[i].writeSignal(VCU2_AC_MaxPowerLimit_LSB, VCU2_AC_MaxPowerLimit_LEN, 0, VCU2_AC_MaxPowerLimit_CONV_D);
        messagesVCU[i].writeSignal(VCU2_RollingCounter_LSB, VCU2_RollingCounter_LEN, 0);
        break;
      case VCU3:
        messagesVCU[i].writeSignal(VCU3_ParkBrake_LSB, VCU3_ParkBrake_LEN, VCU3_ParkBrake_OFF);
        break;
      default:
        report("CAN_MESSAGE_INIT_MISSING", 2);  //TODO
        break;
    }
  }
}
// Default constructor
CanManager::CanManager() {
}


/* ============================== CAN begin ===============================
 * Starts the CAN bus communication with the fixed baud rate and reports
 * the result
 */
void CanManager::begin() {
  if (CAN_OK != _canObj->begin(CAN_500KBPS)) {
    _canObj->wake();
    delay(50);
    _canObj->begin(CAN_500KBPS);
    delay(20);
  }
  if (CanManager::checkError() == CAN_OK) {
    report("CAN_INIT_SUCCESS", 1);  //TODO
  } else {
    report("CAN_BUS_FAULT", 3);  //TODO
  }
}


/* ============================== CAN send message ========================
 * Sends a standard CAN message frame from a set of known message IDs
 * @param id: long identifier of message frame
 * @param interval: (optional) integer milliseconds for periodical send
 * interval. Default 0 for sending it just once.
 */
void CanManager::sendMessage(uint32_t id, int interval) {
  if (CanManager::checkError() == CAN_OK) {
    for (uint8_t i=0; i<N_VCU_MESSAGES; i++) {
      if (messagesVCU[i].getId() == id) {
        messagesVCU[i].send(interval);
      }
    }
  } else {
    CanManager::begin();
  }
}


/* ============================== CAN update ==============================
 * Reads the CAN bus and updates the corresponding message, reacts to
 * signals from other ECUs and sends own messages.
 */
void CanManager::update() {
  if (CanManager::checkError() == CAN_OK) {
    // Flags for occasional messages
    bool adjustThrottlePos = false;
    bool adjustDriveSettings = false;

    // Update info from foreign messages
    if (CAN_MSGAVAIL == _canObj->checkReceive()) {
      byte len;
      byte buf[LSCF];
      _canObj->readMsgBuf(&len, buf);
      uint32_t id = _canObj->getCanId();
      for (uint8_t i=0; i<N_OTHER_MESSAGES; i++) {
        if (messagesOther[i].getId() == id) {

          // Update content
          for (uint8_t j=0; j<min(LSCF, len); j++) {
            messagesOther[i].writeSignal(8*j, 8, buf[j]);
          }

          // List of occasional commands to react to
          if (id == UI_ThrottlePos) {
            adjustThrottlePos = true;
          }
          else if (id == UI_DriveSettings) {
            adjustDriveSettings = true;
          }
        }
      }
    }

    // Communicate with other devices and the UIs
    // Update warning level
    CanManager::writeSignal(VCU1, VCU1_WarningLevel_LSB, VCU1_WarningLevel_LEN, EEPROM.read(VCU1_WARNING_LEVEL));
    
    // React to setting commands
    if (adjustThrottlePos) {
      EEPROM.put(THROTTLE_POS_MAX_TORQUE,
                 (uint8_t)(CanManager::readSignal(UI_ThrottlePos, UI_ThrottlePos_MaxTorque_LSB, UI_ThrottlePos_MaxTorque_LEN)));
      EEPROM.put(THROTTLE_POS_HALF_TORQUE,
                 (uint8_t)(CanManager::readSignal(UI_ThrottlePos, UI_ThrottlePos_HalfTorque_LSB, UI_ThrottlePos_HalfTorque_LEN)));
      EEPROM.put(THROTTLE_POS_ZERO_TORQUE,
                 (uint8_t)(CanManager::readSignal(UI_ThrottlePos, UI_ThrottlePos_ZeroTorque_LSB, UI_ThrottlePos_ZeroTorque_LEN)));
      EEPROM.put(THROTTLE_POS_ZERO_NEG_TORQUE,
                 (uint8_t)(CanManager::readSignal(UI_ThrottlePos, UI_ThrottlePos_ZeroNegTorque_LSB, UI_ThrottlePos_ZeroNegTorque_LEN)));
      EEPROM.put(THROTTLE_POS_MAX_NEG_TORQUE,
                 (uint8_t)(CanManager::readSignal(UI_ThrottlePos, UI_ThrottlePos_HalfNegTorque_LSB, UI_ThrottlePos_HalfNegTorque_LEN)));
      EEPROM.put(THROTTLE_POS_START,
                 (uint8_t)(CanManager::readSignal(UI_ThrottlePos, UI_ThrottlePos_Start_LSB, UI_ThrottlePos_Start_LEN)));
      EEPROM.put(BRAKE_PEDAL_FUNCTION,
                 (uint8_t)(CanManager::readSignal(UI_ThrottlePos, UI_ThrottlePos_BrakePedalFunction_LSB, UI_ThrottlePos_BrakePedalFunction_LEN)));
      adjustThrottlePos = false;
    }
    if (adjustDriveSettings) {
      EEPROM.put(COOL_THRESHOLD,
                 (uint8_t)(CanManager::readSignal(UI_DriveSettings, UI_DriveSettings_CoolThreshold_LSB, UI_DriveSettings_CoolThreshold_LEN, 1, UI_DriveSettings_CoolThreshold_OFFSET)));
      EEPROM.put(MAX_TORQUE,
                 (uint8_t)(CanManager::readSignal(UI_DriveSettings, UI_DriveSettings_MaxTorque_LSB, UI_DriveSettings_MaxTorque_LEN)));
      EEPROM.put(MAX_NEG_TORQUE,
                 (uint8_t)(CanManager::readSignal(UI_DriveSettings, UI_DriveSettings_MaxNegTorque_LSB, UI_DriveSettings_MaxNegTorque_LEN)));
      EEPROM.put(MAX_RPM,
                 (uint8_t)(CanManager::readSignal(UI_DriveSettings, UI_DriveSettings_MaxRPM_LSB, UI_DriveSettings_MaxRPM_LEN)) / 1000);
      EEPROM.put(BRAKE_LIGHT_TORQUE_THRESHOLD,
                 (uint8_t)(CanManager::readSignal(UI_DriveSettings, UI_DriveSettings_BrakeLightTorque_LSB, UI_DriveSettings_BrakeLightTorque_LEN)));
      if (CanManager::readSignal(UI_DriveSettings, UI_DriveSettings_ResetEEPROM_LSB, UI_DriveSettings_ResetEEPROM_LEN) == UI_DriveSettings_ResetEEPROM_ON) {
        resetEEPROM();
      }
      adjustDriveSettings = false;
    }

    //TODO
    // from BMS: VCU2_ChargeEnable_LSB; VCU2_UrgencyCutOffPowerReq_LSB; VCU2_BMS_HeatingCmd_LSB
    // from/to BMS: VCU1_BMS_MainRelayCmd_OFF; VCU1_BMS_AuxRelayCmd_OFF

    // Send own periodical messages
    for (uint8_t i=0; i<N_VCU_MESSAGES; i++) {
      messagesVCU[i].send();
    }
    
  } else {
    CanManager::begin();
  }
}


/* ============================== Check CAN bus state =====================
 * Checks the canObj for errors
 * @return: integer error code. CAN_OK in case all is good
 */
int CanManager::checkError() {
  return _canObj->checkError();
}


/* ============================== Read CAN signals ========================
 * Retrieves the value of one signal from a foreign CAN message frame.
 * @param id: long ID of message to read
 * @param lsb: integer least significant bit
 * @param len: integer length of the signal in bits
 * @param conv: (optional) float conversion factor of physical signal value
 * @param offset: (optional) integer offset of physical signal value
 * @return: float signal content, already converted. -1 as error code
 */
float CanManager::readSignal(uint32_t id, int lsb, int len, float conv, int offset) {
  for (uint8_t i=0; i<N_OTHER_MESSAGES; i++) {
    if (messagesOther[i].getId() == id) {
      return messagesOther[i].readSignal(lsb, len, conv, offset);
    }
  }
  return -1;
}


/* ============================== Write CAN signals =======================
 * Writes a value of one signal to a VCU CAN message.
 * @param id: long ID of message to read
 * @param lsb: integer least significant bit
 * @param len: integer length of the signal in bits
 * @param val: long (4 bytes) value to be written
 * @param conv: (optional) float conversion factor of physical signal value
 * @param offset: (optional) integer offset of physical signal value
 */
void CanManager::writeSignal(uint32_t id, int lsb, int len, long val, float conv, int offset) {
  for (uint8_t i=0; i<N_VCU_MESSAGES; i++) {
    if (messagesVCU[i].getId() == id) {
      messagesVCU[i].writeSignal(lsb, len, val, conv, offset);
    }
  }
}
