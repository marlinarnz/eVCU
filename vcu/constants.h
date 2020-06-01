/* ============================== Constants ===============================
 * All pin declarations, constants, default values, CAN signal structures
 * are saved here.
 */

#ifndef constants_h
#define constants_h

#include "Arduino.h"


/* ============================== Pins ==================================== */
#define throttlePin 111       // Power throttle
#define brakePedalPin 255     // Brake pedal (optional)
#define direcPin 1            // Direction foreward/backward switch
#define recuPin 2             // Brake energy recuperation on/off switch
#define handBrakePin 3        // Hand brake state
#define driverDoorPin 4       // Driver door open/close sensor
#define powerButtonPin 5      // Power button push signal
#define powerButtonLEDPin 6   // Power button internal LED
#define prechargeRelaisPin 7  // Open/close relais for precharge resistor
#define mainContactorPin 8    // Open/close main contactor
#define kl15Pin 9             // For MCU start
#define soundPin 10           // Speaker for acoustic signals
#define selectCANPin 11       // For MCP2515 SPI interface
#define engineFaultLightPin 13// On the warning lights panel
#define coolantTempGaugePin 14// Original gauge on dashboard
#define coolantCyclePin 15    // Controlling the coolant pump
#define fuelGaugePin 16       // Original gauge on dashboard
#define brakeLightPin 17      // To activate brake lights when recu works
//TODO


/* ============================== Constants ==============================
 * Constant values NOT to be changed by the user while vehicle operation
 */
const uint8_t LSCF = 8;       // Length of standard CAN frame
const uint16_t PCDL = 1500;   // Precharge delay: time to delay the motor start
const uint16_t MITL = 2000;   // MCU initialisation time limit
const uint8_t TFDD = 150;     // Tone freuquency driver door: for warn signal
const uint8_t TFHB = 100;     // Tone freuquency hand brake: for warn signal
const uint8_t MTGL = 0;       // Motor temperature gauge lower level
const uint8_t MTGU = 80;      // Motor temperature gauge upper level
const uint8_t BPLC = 5;       // Brake pedal lower cut-off value (%)
const uint8_t BPUC = 99;      // Brake pedal upper cut-off value (%)
const uint8_t TTSF = 5;       // Throttle torque smooth factor (length of array)

// PowerButton settings
const int PBBF = 500;         // blink frequency for LED state 2 (error)
const float PBFF = 0.06;      // brightness change per ms for LED state 1 (off)
const int PBDD = 1000;        // debounce delay for button reading


/* ============================== Report messages ========================
 * The report function fills one CAN frame with messages. The following
 * message IDs are available. With a 6 bit number, there are maximum 63
 * different messages.
 */
#define CONTROLLER_INIT_FAULT 1
#define CONTROLLER_INIT_SUCCESS 2
#define CAN_BUS_FAULT 3
#define CAN_INIT_SUCCESS 4
#define CAN_MESSAGE_INIT_MISSING 5
#define CAN_SIGNAL_WRONG_ID 6
#define CAN_SIGNAL_WRONG_LSB_LEN 7

#define MOTOR_WRONG_DIRECTION 12
#define BRAKE_PEDAL_WRONG_SETTINGS 13


/* ============================== EEPROM =================================
 * These constants are keys to the EEPROM storage byte of the corresponding
 * default value. They represent user settings. Each value should use as
 * many bytes as defined in VAL_N_BYTES.
 */
#define VAL_N_BYTES 1
#define VAL_DATA_TYPE uint8_t

const uint8_t VCU1_WARNING_LEVEL = 0;
const VAL_DATA_TYPE vcu1WarningLevel = 0;
// Motor settings
const uint8_t COOL_THRESHOLD = VCU1_WARNING_LEVEL + VAL_N_BYTES;
const VAL_DATA_TYPE coolThreshold = 40;
const uint8_t MAX_TORQUE = COOL_THRESHOLD + VAL_N_BYTES;
const VAL_DATA_TYPE maxTorque = 255;      // 255 equals 100%
const uint8_t MAX_NEG_TORQUE = MAX_TORQUE + VAL_N_BYTES;
const VAL_DATA_TYPE maxNegTorque = 85;   // TODO: test; 255 equals 100% of maxTorque
const uint8_t MAX_RPM = MAX_NEG_TORQUE + VAL_N_BYTES;
const VAL_DATA_TYPE maxRPM = 6;           // x1000rpm TODO: test
const uint8_t BRAKE_LIGHT_TORQUE_THRESHOLD = MAX_RPM + VAL_N_BYTES;
const VAL_DATA_TYPE brakeLightTorqueThreshold = 10;   // Unit: Nm
// Throttle positions in %, starting with max torque going towards max neg torque
const uint8_t THROTTLE_POS_MAX_TORQUE = BRAKE_LIGHT_TORQUE_THRESHOLD + VAL_N_BYTES;
const VAL_DATA_TYPE throttlePosMaxTorque = 98;
const uint8_t THROTTLE_POS_HALF_TORQUE = THROTTLE_POS_MAX_TORQUE + VAL_N_BYTES;
const VAL_DATA_TYPE throttlePosHalfTorque = 75;
const uint8_t THROTTLE_POS_ZERO_TORQUE = THROTTLE_POS_HALF_TORQUE + VAL_N_BYTES;
const VAL_DATA_TYPE throttlePosZeroTorque = 27;
const uint8_t THROTTLE_POS_ZERO_NEG_TORQUE = THROTTLE_POS_ZERO_TORQUE + VAL_N_BYTES;
const VAL_DATA_TYPE throttlePosZeroNegTorque = 25;
const uint8_t THROTTLE_POS_MAX_NEG_TORQUE = THROTTLE_POS_ZERO_NEG_TORQUE + VAL_N_BYTES;
const VAL_DATA_TYPE throttlePosMaxNegTorque = 13;
const uint8_t THROTTLE_POS_START = THROTTLE_POS_MAX_NEG_TORQUE + VAL_N_BYTES;
const VAL_DATA_TYPE throttlePosStart = 5;
const uint8_t BRAKE_PEDAL_FUNCTION = THROTTLE_POS_START + VAL_N_BYTES;
const VAL_DATA_TYPE brakePedalFunction = 0; // 0 for off; 1 for on (3% = no neg torque; 100% = max neg torque)


/* ============================== CAN messages ===========================
 * Every CAN signal has a least significant bit (LSB; in most cases it
 * represents the start bit) and a length in bits. Additionally, every
 * CAN message has an ID and an optional send interval (0 for none).
 * Names are oriented towards the APEV528 controller product specification.
 */
// VCU messages
#define N_VCU_MESSAGES 3

#define VCU1 0x101
#define VCU1_INTERVAL 20
#define VCU1_TorqueReq_LSB 0
#define VCU1_TorqueReq_LEN 8
#define VCU1_TorqueReq_CONV_D 0.392
#define VCU1_TorqueReq_INVALID 0xFF
#define VCU1_MotorSpdReq_LSB 16
#define VCU1_MotorSpdReq_LEN 16
#define VCU1_MotorSpdReq_CONV_D 0.25
#define VCU1_MotorSpdReq_INVALID 0xFFFF
#define VCU1_Reserved_LSB 24
#define VCU1_Reserved_LEN 5
#define VCU1_AuthenticationStatus_LSB 29
#define VCU1_AuthenticationStatus_LEN 2
#define VCU1_AuthenticationStatus_DEFAULT 0
#define VCU1_AuthenticationStatus_SUCCESS 1
#define VCU1_AuthenticationStatus_FAIL 2
#define VCU1_AuthenticationStatus_RESERVED 3
#define VCU1_ChangeGearAlarm_LSB 31
#define VCU1_ChangeGearAlarm_LEN 1
#define VCU1_ChangeGearAlarm_OK 0
#define VCU1_ChangeGearAlarm_ALARM 1
#define VCU1_VehicleState_LSB 32
#define VCU1_VehicleState_LEN 1
#define VCU1_VehicleState_NOT_READY 0
#define VCU1_VehicleState_READY 1
#define VCU1_BrakePedalSts_LSB 33
#define VCU1_BrakePedalSts_LEN 2
#define VCU1_BrakePedalSts_FREE 0
#define VCU1_BrakePedalSts_PRESSED 1
#define VCU1_BrakePedalSts_RESERVED 2
#define VCU1_BrakePedalSts_ERROR 3
#define VCU1_BMS_MainRelayCmd_LSB 35
#define VCU1_BMS_MainRelayCmd_LEN 1
#define VCU1_BMS_MainRelayCmd_OFF 0
#define VCU1_BMS_MainRelayCmd_ON 1
#define VCU1_GearLeverPosSts_LSB 36
#define VCU1_GearLeverPosSts_LEN 3
#define VCU1_GearLeverPosSts_DEFAULT 0x00
#define VCU1_GearLeverPosSts_R 0x01
#define VCU1_GearLeverPosSts_N 0x02
#define VCU1_GearLeverPosSts_D 0x03
#define VCU1_GearLeverPosSts_P 0x04
#define VCU1_GearLeverPosSts_F_LSB 39
#define VCU1_GearLeverPosSts_F_LEN 1
#define VCU1_GearLeverPosSts_F_OK 0
#define VCU1_GearLeverPosSts_F_ERROR 1
#define VCU1_AC_ControlCmd_LSB 40
#define VCU1_AC_ControlCmd_LEN 1
#define VCU1_AC_ControlCmd_OFF 0
#define VCU1_AC_ControlCmd_ON 1
#define VCU1_WorkMode_LSB 41
#define VCU1_WorkMode_LEN 1
#define VCU1_WorkMode_TORQUE 0
#define VCU1_WorkMode_SPEED 1
#define VCU1_MotorMode_LSB 42
#define VCU1_MotorMode_LEN 2
#define VCU1_MotorMode_STANDBY 0
#define VCU1_MotorMode_POWERDRIVE 1
#define VCU1_MotorMode_GENERATE 2
#define VCU1_WarningLevel_LSB 44
#define VCU1_WarningLevel_LEN 2
#define VCU1_WarningLevel_OK 0
#define VCU1_WarningLevel_WARN1 1
#define VCU1_WarningLevel_WARN2 2
#define VCU1_WarningLevel_WARN3 3
#define VCU1_KeyPosition_LSB 46
#define VCU1_KeyPosition_LEN 2
#define VCU1_KeyPosition_OFF 0
#define VCU1_KeyPosition_ACC 1
#define VCU1_KeyPosition_ON 2
#define VCU1_KeyPosition_CRANK 3
#define VCU1_RollingCounter_LSB 48
#define VCU1_RollingCounter_LEN 4
#define VCU1_BMS_AuxRelayCmd_LSB 52
#define VCU1_BMS_AuxRelayCmd_LEN 1
#define VCU1_BMS_AuxRelayCmd_OFF 0
#define VCU1_BMS_AuxRelayCmd_ON 1
#define VCU1_AC_PowerReduceReq_LSB 53
#define VCU1_AC_PowerReduceReq_LEN 3
#define VCU1_AC_PowerReduceReq_DEFAULT 0
#define VCU1_AC_PowerReduceReq_REDUCE 1
#define VCU1_CheckSum_LSB 56
#define VCU1_CheckSum_LEN 8
#define VCU2 0x102
#define VCU2_INTERVAL 20
#define VCU2_ChargeEnable_LSB 10
#define VCU2_ChargeEnable_LEN 1
#define VCU2_ChargeEnable_OFF 0
#define VCU2_ChargeEnable_ON 1
#define VCU2_UrgencyCutOffPowerReq_LSB 15
#define VCU2_UrgencyCutOffPowerReq_LEN 1
#define VCU2_UrgencyCutOffPowerReq_OK 0
#define VCU2_UrgencyCutOffPowerReq_ERROR 1
#define VCU2_BMS_HeatingCmd_LSB 24
#define VCU2_BMS_HeatingCmd_LEN 1
#define VCU2_BMS_HeatingCmd_OFF 0
#define VCU2_BMS_HeatingCmd_ON 1
#define VCU2_AC_MaxPowerLimit_LSB 32
#define VCU2_AC_MaxPowerLimit_LEN 8
#define VCU2_AC_MaxPowerLimit_CONV_D 0.1
#define VCU2_RollingCounter_LSB 48
#define VCU2_RollingCounter_LEN 4
#define VCU2_CheckSum_LSB 56
#define VCU2_CheckSum_LEN 8
#define VCU3 0x431
#define VCU3_INTERVAL 1000
#define VCU3_ParkBrake_LSB 0
#define VCU3_ParkBrake_LEN 1
#define VCU3_ParkBrake_ON 0
#define VCU3_ParkBrake_OFF 1

#define N_OTHER_MESSAGES 5       // Message storage capacity of CAN manager

// MCU messages
#define MCU1 0x105
#define MCU1_INTERVAL 10
#define MCU1_ActMotorSpd_LSB 8
#define MCU1_ActMotorSpd_LEN 16
#define MCU1_ActMotorSpd_CONV_D 0.25
#define MCU1_ActMotorSpd_INVALID 0xFFFF
#define MCU1_ActMotorTq_LSB 16
#define MCU1_ActMotorTq_LEN 8
#define MCU1_ActMotorTq_CONV_D 0.392
#define MCU1_ActMotorTq_INVALID 0xFF
#define MCU1_MaxMotorTq_LSB 24
#define MCU1_MaxMotorTq_LEN 8
#define MCU1_MaxMotorTq_CONV_D 0.392
#define MCU1_MaxMotorTq_INVALID 0xFF
#define MCU1_MaxMotorBrakeTq_LSB 32
#define MCU1_MaxMotorBrakeTq_LEN 8
#define MCU1_MaxMotorBrakeTq_CONV_D 0.392
#define MCU1_MaxMotorBrakeTq_INVALID 0xFF
#define MCU1_MotorMainState_LSB 43
#define MCU1_MotorMainState_LEN 3
#define MCU1_MotorMainState_STANDBY 0
#define MCU1_MotorMainState_PRECHARGE 1
#define MCU1_MotorMainState_READY 2
#define MCU1_MotorMainState_RUNNING 3
#define MCU1_MotorMainState_OFF 4
#define MCU1_MotorRatoteDirection_LSB 46
#define MCU1_MotorRatoteDirection_LEN 2
#define MCU1_MotorRatoteDirection_STANDBY 0
#define MCU1_MotorRatoteDirection_FORWARD 1
#define MCU1_MotorRatoteDirection_BACKWARD 2
#define MCU1_MotorRatoteDirection_ERROR 3
#define MCU1_RollingCounter_LSB 48
#define MCU1_RollingCounter_LEN 4
#define MCU1_MotorState_LSB 52
#define MCU1_MotorState_LEN 2
#define MCU1_MotorState_STANDBY 0
#define MCU1_MotorState_POWERDRIVE 1
#define MCU1_MotorState_GENERATE 2
#define MCU1_MotorState_RESERVED 3
#define MCU1_MotorWorkMode_LSB 54
#define MCU1_MotorWorkMode_LEN 2
#define MCU1_MotorWorkMode_STANDBY 0
#define MCU1_MotorWorkMode_TORQUE 1
#define MCU1_MotorWorkMode_SPEED 2
#define MCU1_MotorWorkMode_RESERVED 3
#define MCU1_CheckSum_LSB 56
#define MCU1_CheckSum_LEN 8
#define MCU2 0x106
#define MCU2_INTERVAL 50
#define MCU2_MotorTemp_LSB 0
#define MCU2_MotorTemp_LEN 8
#define MCU2_MotorTemp_OFFSET 40
#define MCU2_MotorTemp_INVALID 0xFF
#define MCU2_HardwareTemp_LSB 8
#define MCU2_HardwareTemp_LEN 8
#define MCU2_HardwareTemp_OFFSET 40
#define MCU2_HardwareTemp_INVALID 0xFF
#define MCU2_DC_MainWireOverCurrFault_LSB 16
#define MCU2_DC_MainWireOverCurrFault_LEN 1
#define MCU2_DC_MainWireOverCurrFault_OK 0
#define MCU2_DC_MainWireOverCurrFault_ERROR 1
#define MCU2_MotorPhaseCurrFault_LSB 17
#define MCU2_MotorPhaseCurrFault_LEN 1
#define MCU2_MotorPhaseCurrFault_OK 0
#define MCU2_MotorPhaseCurrFault_ERROR 1
#define MCU2_OverHotFault_LSB 18
#define MCU2_OverHotFault_LEN 1
#define MCU2_OverHotFault_OK 0
#define MCU2_OverHotFault_ERROR 1
#define MCU2_RotateTransformerFault_LSB 19
#define MCU2_RotateTransformerFault_LEN 1
#define MCU2_RotateTransformerFault_OK 0
#define MCU2_RotateTransformerFault_ERROR 1
#define MCU2_PhaseCurrSensorState_LSB 20
#define MCU2_PhaseCurrSensorState_LEN 1
#define MCU2_PhaseCurrSensorState_OK 0
#define MCU2_PhaseCurrSensorState_ERROR 1
#define MCU2_MotorOverSpdFault_LSB 21
#define MCU2_MotorOverSpdFault_LEN 1
#define MCU2_MotorOverSpdFault_OK 0
#define MCU2_MotorOverSpdFault_ERROR 1
#define MCU2_DrvMotorOverHotFault_LSB 22
#define MCU2_DrvMotorOverHotFault_LEN 1
#define MCU2_DrvMotorOverHotFault_OK 0
#define MCU2_DrvMotorOverHotFault_ERROR 1
#define MCU2_DC_MainWireOverVoltFault_LSB 23
#define MCU2_DC_MainWireOverVoltFault_LEN 1
#define MCU2_DC_MainWireOverVoltFault_OK 0
#define MCU2_DC_MainWireOverVoltFault_ERROR 1
#define MCU2_DrvMotorOverCoolFault_LSB 25
#define MCU2_DrvMotorOverCoolFault_LEN 1
#define MCU2_DrvMotorOverCoolFault_OK 0
#define MCU2_DrvMotorOverCoolFault_ERROR 1
#define MCU2_MotorSystemState_LSB 26
#define MCU2_MotorSystemState_LEN 1
#define MCU2_MotorSystemState_OK 0
#define MCU2_MotorSystemState_ERROR 1
#define MCU2_MotorSensorState_LSB 27
#define MCU2_MotorSensorState_LEN 1
#define MCU2_MotorSensorState_OK 0
#define MCU2_MotorSensorState_ERROR 1
#define MCU2_MotorTempSensorState_LSB 28
#define MCU2_MotorTempSensorState_LEN 1
#define MCU2_MotorTempSensorState_OK 0
#define MCU2_MotorTempSensorState_ERROR 1
#define MCU2_DC_VoltSensorState_LSB 29
#define MCU2_DC_VoltSensorState_LEN 1
#define MCU2_DC_VoltSensorState_OK 0
#define MCU2_DC_VoltSensorState_ERROR 1
#define MCU2_DC_LowVoltWarning_LSB 30
#define MCU2_DC_LowVoltWarning_LEN 1
#define MCU2_DC_LowVoltWarning_OK 0
#define MCU2_DC_LowVoltWarning_ERROR 1
#define MCU2_12V_LowVoltWarning_LSB 31
#define MCU2_12V_LowVoltWarning_LEN 1
#define MCU2_12V_LowVoltWarning_OK 0
#define MCU2_12V_LowVoltWarning_ERROR 1
#define MCU2_RollingCounter_LSB 48
#define MCU2_RollingCounter_LEN 4
#define MCU2_WarningLevel_LSB 52
#define MCU2_WarningLevel_LEN 2
#define MCU2_WarningLevel_OK 0
#define MCU2_WarningLevel_ERROR1 1
#define MCU2_WarningLevel_ERROR2 2
#define MCU2_WarningLevel_ERROR3 3
#define MCU2_MotorOpenPhaseFault_LSB 54
#define MCU2_MotorOpenPhaseFault_LEN 1
#define MCU2_MotorOpenPhaseFault_OK 0
#define MCU2_MotorOpenPhaseFault_ERROR 1
#define MCU2_MotorStallFault_LSB 55
#define MCU2_MotorStallFault_LEN 1
#define MCU2_MotorStallFault_OK 0
#define MCU2_MotorStallFault_ERROR 1
#define MCU2_CheckSum_LSB 56
#define MCU2_CheckSum_LEN 8
#define MCU3 0x107
#define MCU3_INTERVAL 50
#define MCU3_DC_MainWireVolt_LSB 8
#define MCU3_DC_MainWireVolt_LEN 16
#define MCU3_DC_MainWireVolt_CONV_D 0.01
#define MCU3_DC_MainWireVolt_INVALID 0xFFFF
#define MCU3_DC_MainWireCurr_LSB 24
#define MCU3_DC_MainWireCurr_LEN 16
#define MCU3_DC_MainWireCurr_CONV_D 0.01
#define MCU3_DC_MainWireCurr_INVALID 0xFFFF
#define MCU3_MotorPhaseCurr_LSB 40
#define MCU3_MotorPhaseCurr_LEN 16
#define MCU3_MotorPhaseCurr_CONV_D 0.01
#define MCU3_MotorPhaseCurr_INVALID 0xFFFF

// BMS messages
#define BMS1 0x520
#define BMS1_INTERVAL 500
#define BMS1_OBC_VoltageSet_LSB 8
#define BMS1_OBC_VoltageSet_LEN 16
#define BMS1_OBC_VoltageSet_CONV_D 0.1
#define BMS1_OBC_CurrentSet_LSB 24
#define BMS1_OBC_CurrentSet_LEN 16
#define BMS1_OBC_CurrentSet_CONV_D 0.1
#define BMS1_OBC_ChargeCommand_LSB 32
#define BMS1_OBC_ChargeCommand_LEN 8
#define BMS1_OBC_ChargeCommand_ON 0
#define BMS1_OBC_ChargeCommand_OFF 1
#define BMS1_OBC_ChargeCommand_HEATING_MODE 2
#define BMS1_OBC_ChargeCommand_COMPLETED 3
#define BMS2 0x1A0
#define BMS2_INTERVAL 20
#define BMS2_PrechargeFinishSts_LSB 28
#define BMS2_PrechargeFinishSts_LEN 1
#define BMS2_PrechargeFinishSts_NOT_FINISHED 0
#define BMS2_PrechargeFinishSts_FINISHED 1
#define BMS2_PrechargeFinishSts_F_LSB 29
#define BMS2_PrechargeFinishSts_F_LEN 1
#define BMS2_PrechargeFinishSts_F_OK 0
#define BMS2_PrechargeFinishSts_F_ERROR 1

// OBC messages
#define OBC1 0x618
#define OBC1_INTERVAL 500
#define OBC1_ChargerVoltage_LSB 8
#define OBC1_ChargerVoltage_LEN 16
#define OBC1_ChargerVoltage_CONV_D 0.1
#define OBC1_ChargerCurrent_LSB 24
#define OBC1_ChargerCurrent_LEN 16
#define OBC1_ChargerCurrent_CONV_D 0.1
#define OBC1_HardwareStatus_LSB 32
#define OBC1_HardwareStatus_LEN 1
#define OBC1_HardwareStatus_OK 0
#define OBC1_HardwareStatus_ERROR 1
#define OBC1_TempAnomaly_LSB 33
#define OBC1_TempAnomaly_LEN 1
#define OBC1_TempAnomaly_OK 0
#define OBC1_TempAnomaly_ERROR 1
#define OBC1_ACVoltageAnomaly_LSB 34
#define OBC1_ACVoltageAnomaly_LEN 1
#define OBC1_ACVoltageAnomaly_OK 0
#define OBC1_ACVoltageAnomaly_ERROR 1
#define OBC1_StartStatus_LSB 35
#define OBC1_StartStatus_LEN 1
#define OBC1_StartStatus_ON 0
#define OBC1_StartStatus_OFF 1
#define OBC1_ComOvertime_LSB 36
#define OBC1_ComOvertime_LEN 1
#define OBC1_ComOvertime_OK 0
#define OBC1_ComOvertime_ERROR 1
#define OBC1_BatteryConnectStatus_LSB 37
#define OBC1_BatteryConnectStatus_LEN 1
#define OBC1_BatteryConnectStatus_OK 0
#define OBC1_BatteryConnectStatus_DISCONNECTED 1
#define OBC1_CCStatus_LSB 38
#define OBC1_CCStatus_LEN 1
#define OBC1_CCStatus_OK 0
#define OBC1_CCStatus_ERROR 1
#define OBC1_CPStatus_LSB 39
#define OBC1_CPStatus_LEN 1
#define OBC1_CPStatus_OK 0
#define OBC1_CPStatus_ERROR 1
#define OBC1_Temperature_LSB 40
#define OBC1_Temperature_LEN 8
#define OBC1_Temperature_OFFSET 40
#define OBC1_SoftwareVersion_LSB 48
#define OBC1_SoftwareVersion_LEN 8
#define OBC1_HardwareVersion_LSB 56
#define OBC1_HardwareVersion_LEN 8
#define OBC2 0x619
#define OBC2_INTERVAL 500
#define OBC2_ACVoltageInput_LSB 8
#define OBC2_ACVoltageInput_LEN 16
#define OBC2_ACVoltageInput_CONV_D 0.1
#define OBC2_PFCVoltage_LSB 24
#define OBC2_PFCVoltage_LEN 16
#define OBC2_PFCVoltage_CONV_D 0.1
#define OBC2_CurrentLimit_LSB 32
#define OBC2_CurrentLimit_LEN 8
#define OBC2_CurrentLimit_CONV_D 0.1
#define OBC2_BatteryVoltage_LSB 40
#define OBC2_BatteryVoltage_LEN 16
#define OBC2_BatteryVoltage_CONV_D 0.1
#define OBC2_SystemStatus_LSB 56
#define OBC2_SystemStatus_LEN 4
#define OBC2_SystemStatus_INIT 0
#define OBC2_SystemStatus_STANDBY 1
#define OBC2_SystemStatus_CHARGE_CC 2
#define OBC2_SystemStatus_CHARGE_CV 3
#define OBC2_SystemStatus_CHARGE_COMPLETE 4
#define OBC2_SystemStatus_SLEEP 5
#define OBC2_SystemStatus_FAULT 6
#define OBC2_SystemStatus_LOCK_FAULT 7
#define OBC2_ErrorFlag_LSB 60
#define OBC2_ErrorFlag_LEN 4
#define OBC2_ErrorFlag_FAULT_PORT_OVER_TEMP 1
#define OBC2_ErrorFlag_FAULT_OVER_TEMP 2
#define OBC2_ErrorFlag_FAULT_OUTPUT_OVER_CURR 3
#define OBC2_ErrorFlag_FAULT_COMM_TIMEOUT 4
#define OBC2_ErrorFlag_FAULT_INPUT_UNDERVOLT 5
#define OBC2_ErrorFlag_FAULT_INPUT_OVERVOLT 6
#define OBC2_ErrorFlag_LOCK_FAULT_PFC_OVERVOLT 8
#define OBC2_ErrorFlag_LOCK_FAULT_PFC_UNDERVOLT 9
#define OBC2_ErrorFlag_LOCK_FAULT_OUTPUT_UNDERVOLT 10
#define OBC2_ErrorFlag_LOCK_FAULT_OUTPUT_OVERVOLT 11
#define OBC3 0x620
#define OBC3_INTERVAL 500
#define OBC3_ACCurrentInput_LSB 8
#define OBC3_ACCurrentInput_LEN 16
#define OBC3_ACCurrentInput_CONV_D 0.1
#define OBC3_ChargingPileMaxCurrent_LSB 24
#define OBC3_ChargingPileMaxCurrent_LEN 16
#define OBC3_ChargingPileMaxCurrent_CONV_D 0.1
#define OBC3_ChargePortTemp1_LSB 32
#define OBC3_ChargePortTemp1_LEN 8
#define OBC3_ChargePortTemp1_OFFSET 40
#define OBC3_ChargePortTemp2_LSB 40
#define OBC3_ChargePortTemp2_LEN 8
#define OBC3_ChargePortTemp2_OFFSET 40
#define OBC3_CPDutyCycle_LSB 48
#define OBC3_CPDutyCycle_LEN 8
#define OBC3_LockStatus_LSB 56
#define OBC3_LockStatus_LEN 1
#define OBC3_LockStatus_ON 0
#define OBC3_LockStatus_OFF 1
#define OBC3_S2Status_LSB 57
#define OBC3_S2Status_LEN 1
#define OBC3_S2Status_DISCONNECTED 0
#define OBC3_S2Status_CLOSED 1
#define OBC3_BMS_WakeStatus_LSB 58
#define OBC3_BMS_WakeStatus_LEN 1
#define OBC3_BMS_WakeStatus_OFF 0
#define OBC3_BMS_WakeStatus_ON 1
#define OBC3_Low12VStatus_LSB 59
#define OBC3_Low12VStatus_LEN 1
#define OBC3_Low12VStatus_OFF 0
#define OBC3_Low12VStatus_ON 1
#define OBC3_OutRelayStatus_LSB 60
#define OBC3_OutRelayStatus_LEN 1
#define OBC3_OutRelayStatus_DISCONNECTED 0
#define OBC3_OutRelayStatus_CLOSED 1
#define OBC3_CCStatus_LSB 61
#define OBC3_CCStatus_LEN 3
#define OBC3_CCStatus_DISCONNECTED 0
#define OBC3_CCStatus_SEMI_JOIN 1
#define OBC3_CCStatus_1500_Ohm 3
#define OBC3_CCStatus_680_Ohm 4
#define OBC3_CCStatus_220_Ohm 5

// UI messages
#define UI_ThrottlePos 0xA01
#define UI_ThrottlePos_INTERVAL 0
#define UI_ThrottlePos_MaxTorque_LSB 0
#define UI_ThrottlePos_MaxTorque_LEN 8
#define UI_ThrottlePos_MaxTorque_INVALID 0xFF
#define UI_ThrottlePos_HalfTorque_LSB 8
#define UI_ThrottlePos_HalfTorque_LEN 8
#define UI_ThrottlePos_HalfTorque_INVALID 0xFF
#define UI_ThrottlePos_ZeroTorque_LSB 16
#define UI_ThrottlePos_ZeroTorque_LEN 8
#define UI_ThrottlePos_ZeroTorque_INVALID 0xFF
#define UI_ThrottlePos_ZeroNegTorque_LSB 24
#define UI_ThrottlePos_ZeroNegTorque_LEN 8
#define UI_ThrottlePos_ZeroNegTorque_INVALID 0xFF
#define UI_ThrottlePos_HalfNegTorque_LSB 32
#define UI_ThrottlePos_HalfNegTorque_LEN 8
#define UI_ThrottlePos_HalfNegTorque_INVALID 0xFF
#define UI_ThrottlePos_Start_LSB 40
#define UI_ThrottlePos_Start_LEN 8
#define UI_ThrottlePos_Start_INVALID 0xFF
#define UI_ThrottlePos_BrakePedalFunction_LSB 48
#define UI_ThrottlePos_BrakePedalFunction_LEN 1
#define UI_ThrottlePos_BrakePedalFunction_OFF 0
#define UI_ThrottlePos_BrakePedalFunction_ON 1
#define UI_ThrottlePos_CheckSum_LSB 56
#define UI_ThrottlePos_CheckSum_LEN 8
#define UI_DriveSettings 0xA02
#define UI_DriveSettings_INTERVAL 0
#define UI_DriveSettings_CoolThreshold_LSB 0
#define UI_DriveSettings_CoolThreshold_LEN 8
#define UI_DriveSettings_CoolThreshold_OFFSET 40
#define UI_DriveSettings_MaxTorque_LSB 16
#define UI_DriveSettings_MaxTorque_LEN 16
#define UI_DriveSettings_MaxNegTorque_LSB 24
#define UI_DriveSettings_MaxNegTorque_LEN 8
#define UI_DriveSettings_MaxRPM_LSB 32
#define UI_DriveSettings_MaxRPM_LEN 8
#define UI_DriveSettings_MaxRPM_CONV_D 1000
#define UI_DriveSettings_BrakeLightTorque_LSB 40
#define UI_DriveSettings_BrakeLightTorque_LEN 8
#define UI_DriveSettings_ResetEEPROM_LSB 63
#define UI_DriveSettings_ResetEEPROM_LEN 1
#define UI_DriveSettings_ResetEEPROM_OFF 0
#define UI_DriveSettings_ResetEEPROM_ON 1

// Various


/* Old message list from APEV528 controller product specification
// VCU messages
#define VCU_3 0x364
#define VCU_3_CycleTime 20
#define VCU_3_MCU_CurrMax 23
#define VCU_3_MCU_CurrMin 23
#define VCU_3_MCU_VoltMin 23
#define VCU_3_RC 23
#define VCU_3_MCU_VoltMax 23
#define VCU_3_WaterTemp2 23
#define VCU_4 0x1b6
#define VCU_4_CycleTime 10
#define VCU_4_MCU_ModeReq 23
#define VCU_4_MCU_ModeReq_Inv 23
#define VCU_4_MCU_TorqSet 23
#define VCU_4_MCU_TorqSet_Rdd 23
#define VCU_4_MCU_TorqMax 23
#define VCU_4_MCU_TorqMin 23
#define VCU_4_RC 23
#define VCU_4_MCU_SpdSet 23
#define VCU_6 0x2a1
#define VCU_6_CycleTime 20
#define VCU_6_BMS_RlyClsReq 23
#define VCU_6_KeySt 23
#define VCU_6_RC 23

// MCU messages
#define MCU_1 0x171
#define MCU_1_CycleTime 10
#define MCU_1_RotSpd_Act 23
#define MCU_1_RotSpdAct_Red 23
#define MCU_1_TorqAct 23
#define MCU_1_ModeSt 23
#define MCU_1_TorqAct_Red 23
#define MCU_1_RC 23
#define MCU_1_GateDrvSt 23
#define MCU_1_SysTempOvrInd 23
#define MCU_1_SysTempOvrInd_len 23
#define MCU_1_SysTempOvrInd_TRUE 1
#define MCU_1_SysTempOvrInd_FALSE 0
#define MCU_2 0x179
#define MCU_2_CycleTime 100
#define MCU_2_MotTempAct 23
#define MCU_2_RotTempAct 23
#define MCU_2_InvtTempAct 23
#define MCU_2_DeratSt 23
#define MCU_2_RC 23
#define MCU_2_OffsetcalReq 23
#define MCU_2_OffsetCalInd 23
#define MCU_2_RepairInd 23
#define MCU_2_RepairInd_len 23
#define MCU_2_RepairInd_TRUE 1
#define MCU_2_RepairInd_FALSE 0
#define MCU_2_Id 23
#define MCU_2_Iq 23
#define MCU_3 0x181
#define MCU_3_CycleTime 10
#define MCU_3_TorqMin_Red 23
#define MCU_3_TorqMin 23
#define MCU_3_TorqMax_Red 23
#define MCU_3_HvRdySt 23
#define MCU_3_RC 23
#define MCU_3_CurrAct 23
#define MCU_3_ErrLvl 23
#define MCU_3_ErrLvl_Inv 23
#define MCU_3_VoltAct 23
#define MCU_4 0x189
#define MCU_4_CycleTime 100
#define MCU_4_RC 23
#define MCU_4_FaultLampInd 23
#define MCU_4_FaultLampInd_len 23
#define MCU_4_FaultLampInd_TRUE 1
#define MCU_4_FaultLampInd_FALSE 0
#define MCU_4_Fault1 23
#define MCU_4_Fault1_len 23
#define MCU_4_Fault2 23
#define MCU_4_Fault2_len 23
#define MCU_4_Fault3 23
#define MCU_4_Fault3_len 23
#define MCU_5 0x398
#define MCU_5_CycleTime 1000
#define MCU_5_ID 23
#define MCU_5_SV 23
#define MCU_5_RC 23
#define MCU_5_OffsetAngle 23
#define MCU_Diag_Tx 0x786

// Various
#define MCU_Phys_Diag 0x706
#define Func_Diag_Rx 0x7df*/

#endif
 
