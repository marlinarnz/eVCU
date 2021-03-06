/* ============================== CAN Manager ============================= 
 * The CanManager class handles all CAN bus communication and saves messages
 * from other ECUs.
 */

#include "CanManager.h"


/* ============================== CAN constructor =========================
 * The constructor saves the given CAN object and instantiates CAN messages
 * for every known ID as own attributes.
 * @param canObj: MCP2515 class object pointer
 */
CanManager::CanManager(MCP2515* canObj)
  : _canObj(canObj),
  messagesVCU{
    {VCU1, _canObj, VCU1_INTERVAL},
    {VCU2, _canObj, VCU2_INTERVAL},
    {VCU3, _canObj, VCU3_INTERVAL}
  }, messagesOther{
    {MCU1, _canObj, MCU1_INTERVAL},
    {MCU2, _canObj, MCU2_INTERVAL},
    {MCU3, _canObj, MCU3_INTERVAL},
    {UI_ThrottlePos, _canObj, UI_ThrottlePos_INTERVAL},
    {UI_DriveSettings, _canObj, UI_DriveSettings_INTERVAL}
  } {
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
CanManager::CanManager() 
  : _canObj(NULL), messagesVCU{}, messagesOther{} {
}


/* ============================== CAN begin ===============================
 * Starts the CAN bus communication with the fixed baud rate and reports
 * the result
 */
void CanManager::begin() {
  _canObj->reset();
  _canObj->setBitrate(CAN_500KBPS, MCP_8MHZ);
  _canObj->setNormalMode();
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
    report("CAN_BUS_FAULT", 3); //TODO
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
    if (_canObj->checkReceive()) {
      struct can_frame frame;
      _canObj->readMessage(&frame);
      for (uint8_t i=0; i<N_OTHER_MESSAGES; i++) {
        if (messagesOther[i].getId() == frame.can_id) {

          // Update content
          for (uint8_t j=0; j<min(LSCF, frame.can_dlc); j++) {
            messagesOther[i].writeByte(j, frame.data[j]);
          }

          // List of occasional commands to react to
          if (frame.can_id == UI_ThrottlePos) {
            adjustThrottlePos = true;
          }
          else if (frame.can_id == UI_DriveSettings) {
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
    report("CAN_BUS_FAULT", 3); //TODO
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


/* ============================== Check MCU state =========================
 * Checks whether the MCU is ready
 * @param quick: bool if quick check or thorough
 * @return: bool if device is ready or not
 */
bool CanManager::checkMCUready(bool quick) {
  if (CanManager::checkError() == CAN_OK
    && CanManager::readSignal(MCU2, MCU2_WarningLevel_LSB, MCU2_WarningLevel_LEN) <= MCU2_WarningLevel_ERROR1
    && CanManager::readSignal(MCU2, MCU2_MotorSystemState_LSB, MCU2_MotorSystemState_LEN) == MCU2_MotorSystemState_OK) {
    if (!quick && 
      (CanManager::readSignal(MCU2, MCU2_DC_MainWireOverCurrFault_LSB, MCU2_DC_MainWireOverCurrFault_LEN) == MCU2_DC_MainWireOverCurrFault_ERROR
      || CanManager::readSignal(MCU2, MCU2_MotorPhaseCurrFault_LSB, MCU2_MotorPhaseCurrFault_LEN) == MCU2_MotorPhaseCurrFault_ERROR
      || CanManager::readSignal(MCU2, MCU2_OverHotFault_LSB, MCU2_OverHotFault_LEN) == MCU2_OverHotFault_ERROR
      || CanManager::readSignal(MCU2, MCU2_RotateTransformerFault_LSB, MCU2_RotateTransformerFault_LEN) == MCU2_RotateTransformerFault_ERROR
      || CanManager::readSignal(MCU2, MCU2_MotorOverSpdFault_LSB, MCU2_MotorOverSpdFault_LEN) == MCU2_MotorOverSpdFault_ERROR
      || CanManager::readSignal(MCU2, MCU2_DrvMotorOverHotFault_LSB, MCU2_DrvMotorOverHotFault_LEN) == MCU2_DrvMotorOverHotFault_ERROR
      || CanManager::readSignal(MCU2, MCU2_DC_MainWireOverVoltFault_LSB, MCU2_DC_MainWireOverVoltFault_LEN) == MCU2_DC_MainWireOverVoltFault_ERROR
      || CanManager::readSignal(MCU2, MCU2_DrvMotorOverCoolFault_LSB, MCU2_DrvMotorOverCoolFault_LEN) == MCU2_DrvMotorOverCoolFault_ERROR
      || CanManager::readSignal(MCU2, MCU2_MotorOpenPhaseFault_LSB, MCU2_MotorOpenPhaseFault_LEN) == MCU2_MotorOpenPhaseFault_ERROR)) {
        return false;
      }
    return true;
    }
  return false;
}


/* ============================== Check BMS state =========================
 * Checks whether the BMS is ready
 * @param quick: bool if quick check or thorough
 * @return: bool if device is ready or not
 */

//TODO

bool CanManager::checkBMSready(bool quick) {
  return CanManager::checkError() == CAN_OK;
         //&& CanManager::readSignal(MCU2, MCU2_WarningLevel_LSB, MCU2_WarningLevel_LEN) <= MCU2_WarningLevel_ERROR1
         //&& CanManager::readSignal(MCU2, MCU2_MotorSystemState_LSB, MCU2_MotorSystemState_LEN) == MCU2_MotorSystemState_OK
         //&& quick ? true :
         //! (CanManager::readSignal(MCU2, MCU2_DC_MainWireOverCurrFault_LSB, MCU2_DC_MainWireOverCurrFault_LEN) == MCU2_DC_MainWireOverCurrFault_ERROR
         //|| CanManager::readSignal(MCU2, MCU2_MotorOpenPhaseFault_LSB, MCU2_MotorOpenPhaseFault_LEN) == MCU2_MotorOpenPhaseFault_ERROR);
}


/* ============================== Check MCU warnings ======================
 * Checks whether the MCU has warnings that do not affect the driving
 * capabilities (yet)
 * @return: bool if MCU has something to warn about
 */
bool CanManager::checkMCUwarning() {
  return CanManager::readSignal(MCU1, MCU1_ActMotorSpd_LSB, MCU1_ActMotorSpd_LEN) == MCU1_ActMotorSpd_INVALID
    || CanManager::readSignal(MCU1, MCU1_ActMotorTq_LSB, MCU1_ActMotorTq_LEN) == MCU1_ActMotorTq_INVALID
    || CanManager::readSignal(MCU1, MCU1_MaxMotorTq_LSB, MCU1_MaxMotorTq_LEN) == MCU1_MaxMotorTq_INVALID
    || CanManager::readSignal(MCU1, MCU1_MaxMotorBrakeTq_LSB, MCU1_MaxMotorBrakeTq_LEN) == MCU1_MaxMotorBrakeTq_INVALID
    || CanManager::readSignal(MCU1, MCU1_MaxMotorTq_LSB, MCU1_MaxMotorTq_LEN) > EEPROM.read(MAX_TORQUE)
    || CanManager::readSignal(MCU1, MCU1_MaxMotorBrakeTq_LSB, MCU1_MaxMotorBrakeTq_LEN) > EEPROM.read(MAX_NEG_TORQUE)
    || CanManager::readSignal(MCU1, MCU1_MotorRatoteDirection_LSB, MCU1_MotorRatoteDirection_LEN) == MCU1_MotorRatoteDirection_ERROR
    || CanManager::readSignal(MCU2, MCU2_MotorTemp_LSB, MCU2_MotorTemp_LEN) == MCU2_MotorTemp_INVALID
    || CanManager::readSignal(MCU2, MCU2_HardwareTemp_LSB, MCU2_HardwareTemp_LEN) == MCU2_HardwareTemp_INVALID
    || CanManager::readSignal(MCU2, MCU2_PhaseCurrSensorState_LSB, MCU2_PhaseCurrSensorState_LEN) == MCU2_PhaseCurrSensorState_ERROR
    || CanManager::readSignal(MCU2, MCU2_MotorSensorState_LSB, MCU2_MotorSensorState_LEN) == MCU2_MotorSensorState_ERROR
    || CanManager::readSignal(MCU2, MCU2_DC_VoltSensorState_LSB, MCU2_DC_VoltSensorState_LEN) == MCU2_DC_VoltSensorState_ERROR
    || CanManager::readSignal(MCU2, MCU2_DC_LowVoltWarning_LSB, MCU2_DC_LowVoltWarning_LEN) == MCU2_DC_LowVoltWarning_ERROR
    || CanManager::readSignal(MCU2, MCU2_12V_LowVoltWarning_LSB, MCU2_12V_LowVoltWarning_LEN) == MCU2_12V_LowVoltWarning_ERROR
    || CanManager::readSignal(MCU2, MCU2_WarningLevel_LSB, MCU2_WarningLevel_LEN) >= MCU2_WarningLevel_ERROR1
    || CanManager::readSignal(MCU2, MCU2_MotorStallFault_LSB, MCU2_MotorStallFault_LEN) == MCU2_MotorStallFault_ERROR
    || CanManager::readSignal(MCU3, MCU3_DC_MainWireVolt_LSB, MCU3_DC_MainWireVolt_LEN) == MCU3_DC_MainWireVolt_INVALID
    || CanManager::readSignal(MCU3, MCU3_DC_MainWireCurr_LSB, MCU3_DC_MainWireCurr_LEN) == MCU3_DC_MainWireCurr_INVALID
    || CanManager::readSignal(MCU3, MCU3_MotorPhaseCurr_LSB, MCU3_MotorPhaseCurr_LEN) == MCU3_MotorPhaseCurr_INVALID;
}


/* ============================== Motor status ============================
 * Returns the motor state
 * @return: int motor state (see constants)
 */
int CanManager::getMotorState() {
  
  //TODO what does MotorState/MotorMainState_STANDBY and READY mean?
  return int(CanManager::readSignal(MCU1, MCU1_MotorMainState_LSB, MCU1_MotorMainState_LEN));
}


/* ============================== Motor temperature =======================
 * Returns the motor temperature
 * @return: int motor temoerature
 */
int CanManager::getMotorTemp() {
  return int(CanManager::readSignal(MCU2, MCU2_MotorTemp_LSB, MCU2_MotorTemp_LEN, 1, MCU2_MotorTemp_OFFSET));
}


/* ============================== Controller temperature ==================
 * Returns the controller temperature
 * @return: int controller temoerature
 */
int CanManager::getControllerTemp() {
  return int(CanManager::readSignal(MCU2, MCU2_HardwareTemp_LSB, MCU2_HardwareTemp_LEN, 1, MCU2_HardwareTemp_OFFSET));
}


/* ============================== Motor torque ============================
 * Returns the motor torque
 * @return: int motor torque
 */
int CanManager::getMotorTorque() {
  return int(CanManager::readSignal(MCU1, MCU1_ActMotorTq_LSB, MCU1_ActMotorTq_LEN, MCU1_ActMotorTq_CONV_D));
}


/* ============================== Motor speed =============================
 * Returns the motor speed
 * @return: int motor speed
 */
int CanManager::getMotorSpeed() {
  return int(CanManager::readSignal(MCU1, MCU1_ActMotorSpd_LSB, MCU1_ActMotorSpd_LEN, MCU1_ActMotorSpd_CONV_D));
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
      float sig;
      if (lsb % 8 == 0 && len == 8) {
        sig = messagesOther[i].readByte(int(lsb / 8), conv, offset);
      } else {
        sig = messagesOther[i].readSignalBE(lsb, len, conv, offset);
      }
      if (sig == -1 || (millis() - messagesOther[i].getLastUpdateTime() > 
            messagesOther[i].getInterval() * CMIF)) {
        report("CAN_SIGNAL_ERROR", 2);
        //TODO: List of critical messages
        if (id == MCU1 || id == MCU2 || id == MCU3 || id == BMS1 || id == BMS2) {
          sig = -1;
        }
      }
      return sig;
    }
  }
  report("CAN_SIGNAL_WRONG_ID", 2);
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
      if (lsb % 8 == 0 && len == 8) {
        messagesVCU[i].writeByte(int(lsb / 8), val, conv, offset);
      } else {
        messagesVCU[i].writeSignal(lsb, len, val, conv, offset);
      }
    }
  }
}
