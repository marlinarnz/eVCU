/* ============================== test CAN messages ======================= 
 * Tests the MCU messages' validity and prints them out using the VCU's
 * constants and CanMessage class.
 */

#include "constants.h"
#include "CanMessage.h"
#include <SPI.h>
#include <mcp2515.h>		// MCP2515 driver

// Set up the CAN interface
MCP2515 CAN(10);			// Set SPI Chip Select

// Struct to store frames
struct can_frame frame;

// Interactive pins
const int PIN_ON_OFF = 22;
const int PIN_ACC_CRANK = 23;
const int PIN_VEH_STATE = 24;
const int PIN_AUTH_STATE = 25;
const int PIN_GEAR_D_R = 26;
const int PIN_WARN_LVL = 28;
// Potis
const int POTI_1 = 6;
const int POTI_2 = 9;

// Number of messages to print
const int N_MSG = 3;
// Message array
CanMessage msgs[N_MSG];

// Instantiate messages which the MCU reacts to
CanMessage msgToMCU(VCU1, &CAN, VCU1_INTERVAL);


// Print a CAN message
void printMessage(uint32_t id, uint8_t len, uint8_t *frame) {
	// General
	Serial.println("Message ID: " + String(id, HEX) + ", frame length: " + String(len));
	
	// Print content as hex
	Serial.print("Content: ");
	for(int i=0; i<len; i++) {
		char hexString[20];
		sprintf(hexString, "%X", frame[i]);	// Convert number to hex
		Serial.print(String(hexString) + " ");
	}
	Serial.println("");
	
	// Instantiate the CanMessage object to read signals
	CanMessage msg(id, &CAN, 0);
	for(int i=0; i<len; i++) {
		msg.writeSignal(8*i, 8, frame[i]);
	}
	
	// Translate signals
	switch(id) {
		case MCU1:
			Serial.println("RPM: " + String(msg.readSignalBE(MCU1_ActMotorSpd_LSB, MCU1_ActMotorSpd_LEN, MCU1_ActMotorSpd_CONV_D)));
			Serial.println("Torque: " + String(msg.readSignalBE(MCU1_ActMotorTq_LSB, MCU1_ActMotorTq_LEN, MCU1_ActMotorTq_CONV_D)));
			Serial.println("Max torque: " + String(msg.readSignalBE(MCU1_MaxMotorTq_LSB, MCU1_MaxMotorTq_LEN, MCU1_MaxMotorTq_CONV_D)));
			Serial.println("Max neg. torque: " + String(msg.readSignalBE(MCU1_MaxMotorBrakeTq_LSB, MCU1_MaxMotorBrakeTq_LEN, MCU1_MaxMotorBrakeTq_CONV_D)));
			Serial.println("Motor main state: " + String(msg.readSignalBE(MCU1_MotorMainState_LSB, MCU1_MotorMainState_LEN)));
			Serial.println("Rotate direction: " + String(msg.readSignalBE(MCU1_MotorRatoteDirection_LSB, MCU1_MotorRatoteDirection_LEN)));
			Serial.println("Motor state: " + String(msg.readSignalBE(MCU1_MotorState_LSB, MCU1_MotorState_LEN)));
			Serial.println("Motor work mode: " + String(msg.readSignalBE(MCU1_MotorWorkMode_LSB, MCU1_MotorWorkMode_LEN)));
     Serial.println("Check sum: " + String(msg.readSignalBE(MCU1_CheckSum_LSB, MCU1_CheckSum_LEN)));
     Serial.println("Rolling counter: " + String(msg.readSignalBE(MCU1_RollingCounter_LSB, MCU1_RollingCounter_LEN)));
			break;
		case MCU2:
			Serial.println("Motor temp: " + String(msg.readSignalBE(MCU2_MotorTemp_LSB, MCU2_MotorTemp_LEN, 1, MCU2_MotorTemp_OFFSET)));
			Serial.println("Controller temp: " + String(msg.readSignalBE(MCU2_HardwareTemp_LSB, MCU2_HardwareTemp_LEN, 1, MCU2_HardwareTemp_OFFSET)));
			Serial.println("Main wire over current fault: " + String(msg.readSignalBE(MCU2_DC_MainWireOverCurrFault_LSB, MCU2_DC_MainWireOverCurrFault_LEN)));
			Serial.println("Motor phase current fault: " + String(msg.readSignalBE(MCU2_MotorPhaseCurrFault_LSB, MCU2_MotorPhaseCurrFault_LEN)));
     Serial.println("Controller overheating fault: " + String(msg.readSignalBE(MCU2_OverHotFault_LSB, MCU2_OverHotFault_LEN)));
     Serial.println("Rotate transformer fault: " + String(msg.readSignalBE(MCU2_RotateTransformerFault_LSB, MCU2_RotateTransformerFault_LEN)));
     Serial.println("Phase current sensor fault: " + String(msg.readSignalBE(MCU2_PhaseCurrSensorState_LSB, MCU2_PhaseCurrSensorState_LEN)));
     Serial.println("Max RPM fault: " + String(msg.readSignalBE(MCU2_MotorOverSpdFault_LSB, MCU2_MotorOverSpdFault_LEN)));
     Serial.println("Motor overheating fault: " + String(msg.readSignalBE(MCU2_DrvMotorOverHotFault_LSB, MCU2_DrvMotorOverHotFault_LEN)));
     Serial.println("Main wire over voltage: " + String(msg.readSignalBE(MCU2_DC_MainWireOverVoltFault_LSB, MCU2_DC_MainWireOverVoltFault_LEN)));
     Serial.println("Motor under temperature fault: " + String(msg.readSignalBE(MCU2_DrvMotorOverCoolFault_LSB, MCU2_DrvMotorOverCoolFault_LEN)));
     Serial.println("Motor system state: " + String(msg.readSignalBE(MCU2_MotorSystemState_LSB, MCU2_MotorSystemState_LEN)));
     Serial.println("Motor sensor state: " + String(msg.readSignalBE(MCU2_MotorSensorState_LSB, MCU2_MotorSensorState_LEN)));
     Serial.println("Motor temperature sensor state: " + String(msg.readSignalBE(MCU2_MotorTempSensorState_LSB, MCU2_MotorTempSensorState_LEN)));
     Serial.println("DC volt sensor state: " + String(msg.readSignalBE(MCU2_DC_VoltSensorState_LSB, MCU2_DC_VoltSensorState_LEN)));
     Serial.println("DC low under voltage warning: " + String(msg.readSignalBE(MCU2_DC_LowVoltWarning_LSB, MCU2_DC_LowVoltWarning_LEN)));
     Serial.println("12V under voltage warning: " + String(msg.readSignalBE(MCU2_12V_LowVoltWarning_LSB, MCU2_12V_LowVoltWarning_LEN)));
     Serial.println("Controller warning level: " + String(msg.readSignalBE(MCU2_WarningLevel_LSB, MCU2_WarningLevel_LEN)));
     Serial.println("Motor open phase fault: " + String(msg.readSignalBE(MCU2_MotorOpenPhaseFault_LSB, MCU2_MotorOpenPhaseFault_LEN)));
     Serial.println("Motor stall fault: " + String(msg.readSignalBE(MCU2_MotorStallFault_LSB, MCU2_MotorStallFault_LEN)));
     Serial.println("Check sum: " + String(msg.readSignalBE(MCU2_CheckSum_LSB, MCU2_CheckSum_LEN)));
     Serial.println("Rolling counter: " + String(msg.readSignalBE(MCU2_RollingCounter_LSB, MCU2_RollingCounter_LEN)));
			break;
		case MCU3:
			Serial.println("Main wire voltage: " + String(msg.readSignalBE(MCU3_DC_MainWireVolt_LSB, MCU3_DC_MainWireVolt_LEN, MCU3_DC_MainWireVolt_CONV_D)));
			Serial.println("Main wire current: " + String(msg.readSignalBE(MCU3_DC_MainWireCurr_LSB, MCU3_DC_MainWireCurr_LEN, MCU3_DC_MainWireCurr_CONV_D)));
			Serial.println("Motor phase current: " + String(msg.readSignalBE(MCU3_MotorPhaseCurr_LSB, MCU3_MotorPhaseCurr_LEN, MCU3_MotorPhaseCurr_CONV_D)));
			break;
		default:
			Serial.println("Unknown message ID");
			break;
	}
}

// Read a on/off pin
int getSwitchPos(int pin) {
	int val = digitalRead(pin);
	if(val==LOW) {
		return 0;
	} else {
	  return 1;
	}
}

// Read poti position
int getPotiPos(int pin, int lower, int upper) {
  float pos = analogRead(pin) / 1023;
  int val = int(lower + pos * (upper - lower));
  //Serial.println("Poti " + String(pin) + " value: " + String(val));
  return val;
}


void setup() {
	// Set digital pins
  pinMode(PIN_ON_OFF, INPUT);
  pinMode(PIN_ACC_CRANK, INPUT);
  //pinMode(PIN_VEH_STATE, INPUT);
  //pinMode(PIN_AUTH_STATE, INPUT);
  pinMode(PIN_GEAR_D_R, INPUT);
  //pinMode(PIN_WARN_LVL, INPUT);

  Serial.begin(115200);
  SPI.begin();

  // Fill the messages array
  msgs[0] = {MCU1, &CAN, MCU1_INTERVAL};
  msgs[1] = {MCU2, &CAN, MCU2_INTERVAL};
  msgs[2] = {MCU3, &CAN, MCU3_INTERVAL};

  // Set initial values of the VCU frame if different from 0
  msgToMCU.writeSignal(VCU1_AuthenticationStatus_LSB,
    VCU1_AuthenticationStatus_LEN,
    VCU1_AuthenticationStatus_SUCCESS);
  msgToMCU.writeSignal(VCU1_VehicleState_LSB,
    VCU1_VehicleState_LEN,
    VCU1_VehicleState_READY);
	
	// Start the CAN bus communication
	Serial.println("Starting the MCP2515");
	CAN.reset();
  CAN.setBitrate(CAN_500KBPS, MCP_8MHZ);
  CAN.setNormalMode();
	Serial.println("Done.");
}
	
// Check the bus in an infinite loop
void loop() {
	if(MCP2515::ERROR_OK == CAN.readMessage(&frame)) {//check if data coming

		// Print if something new appears
		for(int msg=0; msg<N_MSG; msg++) {
			if(msgs[msg].getId()==frame.can_id) {
				for(int i=0; i<min(frame.can_dlc, 6); i++) {
					if(msgs[msg].readByte(i)!=frame.data[i]) {
            printMessage(frame.can_id, frame.can_dlc, frame.data);
				    // Update the msgs array
						for(int j=0; j<frame.can_dlc; j++) {
							msgs[msg].writeByte(j, frame.data[j]);
						}
					}
				}
			}
		}
	}
	
	// Update VCU signal
  // Key position acc/on
  int on_off = 1;
  if(getSwitchPos(PIN_ON_OFF)) {
    on_off = 2;
  }
  // Key position crank
  if(getSwitchPos(PIN_ACC_CRANK)) {
    on_off = 3;
  }
  msgToMCU.writeSignal(VCU1_KeyPosition_LSB,
    VCU1_KeyPosition_LEN,
    on_off);

  // Direction
  int direc = 3;
  if(getSwitchPos(PIN_GEAR_D_R)) {
    direc = 1;
  }
  msgToMCU.writeSignal(VCU1_GearLeverPosSts_LSB,
    VCU1_GearLeverPosSts_LEN,
    direc);

  // Torque request
  msgToMCU.writeSignal(VCU1_TorqueReq_LSB,
    VCU1_TorqueReq_LEN,
    getPotiPos(POTI_1, 0, 2),
    VCU1_TorqueReq_CONV_D);
	
	// Write and send the BMS signal
	msgToMCU.send();
}
