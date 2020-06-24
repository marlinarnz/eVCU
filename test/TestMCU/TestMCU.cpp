/* ============================== test CAN messages ======================= 
 * Tests the MCU messages' validity and prints them out using the VCU's
 * constants and CanMessage class.
 */

#include "TestMCU.h"
#include "constants.h"
#include "CanMessage.h"
#include <wiringPi.h>		// Library to use C code on the Pi's GPIO pins
#include <mcp_can.h>		// MCP2515 driver
#include <iostream>			// Command line interaction

using namespace std;

// Set up the CAN interface
// https://www.raspberrypi.org/documentation/usage/gpio/
// https://www.digikey.com/en/maker/blogs/2019/how-to-use-gpio-on-the-raspberry-pi-with-c
// https://www.raspberrypi.org/forums/viewtopic.php?f=44&t=141052
// https://crycode.de/can-bus-am-raspberry-pi
wiringPiSetupGpio();
MCP_CAN CAN(24);			// Set SPI Chip Select

// Interactive pins
const int PIN_ON_OFF = 23;
const int PIN_ACC_CRANK = 18;
const int PIN_RELAIS_ON_OFF = 16;
const int PIN_MOTOR_ON_OFF = 17;

// Number of messages to print
const int N_MSG = 3
// Message array
CanMessage msgs{
	{MCU1, CAN, MCU1_INTERVAL},
	{MCU2, CAN, MCU2_INTERVAL},
	{MCU3, CAN, MCU3_INTERVAL}
};

// Instantiate messages which the MCU reacts to
CanMessage vcu1(VCU1, CAN, VCU1_INTERVAL);
vcu1.writeSignal(VCU1_VehicleState_LSB,
	VCU1_VehicleState_LEN,
	VCU1_VehicleState_READY);
vcu1.writeSignal(VCU1_BMS_MainRelayCmd_LSB,
	VCU1_BMS_MainRelayCmd_LEN,
	VCU1_BMS_MainRelayCmd_OFF);
vcu1.writeSignal(VCU1_MotorMode_LSB,
	VCU1_MotorMode_LEN,
	VCU1_MotorMode_STANDBY);
vcu1.writeSignal(VCU1_KeyPosition_LSB,
	VCU1_KeyPosition_LEN,
	VCU1_KeyPosition_OFF);


// Print a CAN message
void printMessage(uint32_t id, uint8_t len, uint8_t *frame) {
	// General
	cout<<"Message ID: "<<id<<", frame length: "<<len<<endl;
	
	// Print content as hex
	cout<<"Content: ";
	for(int i=0; i<len; i++) {
		char hexString[20];
		sprintf(hex_string, "%X", frame[i]);	// Convert number to hex
		cout<<hexString<<" ";
	}
	cout<<endl;
	
	// Instantiate the CanMessage object to read signals
	CanMessage msg(id, CAN, 0);
	for(int i=0; i<len; i++) {
		msg.writeSignal(8*i, 8, frame[i]);
	}
	
	// Translate signals
	switch(id) {
		case OBC1:
			cout<<"Voltage: "<<msg.readSignalBE(OBC1_ChargerVoltage_LSB, OBC1_ChargerVoltage_LEN, OBC1_ChargerVoltage_CONV_D)<<endl;
			cout<<"Current: "<<msg.readSignalBE(OBC1_ChargerCurrent_LSB, OBC1_ChargerCurrent_LEN, OBC1_ChargerCurrent_CONV_D)<<endl;
			cout<<"Hardware status (0=OK): "<<msg.readSignalBE(OBC1_HardwareStatus_LSB, OBC1_HardwareStatus_LEN)<<endl;
			cout<<"Temperature status (0=OK): "<<msg.readSignalBE(OBC1_TempAnomaly_LSB, OBC1_TempAnomaly_LEN)<<endl;
			cout<<"AC Voltage status (0=OK): "<<msg.readSignalBE(OBC1_ACVoltageAnomaly_LSB, OBC1_ACVoltageAnomaly_LEN)<<endl;
			cout<<"Start status (0=OK): "<<msg.readSignalBE(OBC1_StartStatus_LSB, OBC1_StartStatus_LEN)<<endl;
			cout<<"Communication overtime (0=OK): "<<msg.readSignalBE(OBC1_ComOvertime_LSB, OBC1_ComOvertime_LEN)<<endl;
			cout<<"Battery connection (0=OK): "<<msg.readSignalBE(OBC1_BatteryConnectStatus_LSB, OBC1_BatteryConnectStatus_LEN)<<endl;
			cout<<"CC status (0=OK): "<<msg.readSignalBE(OBC1_CCStatus_LSB, OBC1_CCStatus_LEN)<<endl;
			cout<<"CP status (0=OK): "<<msg.readSignalBE(OBC1_CPStatus_LSB, OBC1_CPStatus_LEN)<<endl;
			cout<<"Temperature: "<<msg.readSignalBE(OBC1_Temperature_LSB, OBC1_Temperature_LEN, 1, OBC1_Temperature_OFFSET)<<endl;
			cout<<"Software version: "<<msg.readSignalBE(OBC1_SoftwareVersion_LSB, OBC1_SoftwareVersion_LEN)<<endl;
			cout<<"Hardware version: "<<msg.readSignalBE(OBC1_HardwareVersion_LSB, OBC1_HardwareVersion_LEN)<<endl;
			break;
		case OBC2:
			cout<<"AC voltage input: "<<msg.readSignalBE(OBC2_ACVoltageInput_LSB, OBC2_ACVoltageInput_LEN, OBC2_ACVoltageInput_CONV_D)<<endl;
			cout<<"PFC voltage: "<<msg.readSignalBE(OBC2_PFCVoltage_LSB, OBC2_PFCVoltage_LEN, OBC2_PFCVoltage_CONV_D)<<endl;
			cout<<"Current limit: "<<msg.readSignalBE(OBC2_CurrentLimit_LSB, OBC2_CurrentLimit_LEN, OBC2_CurrentLimit_CONV_D)<<endl;
			cout<<"Battery voltage: "<<msg.readSignalBE(OBC2_BatteryVoltage_LSB, OBC2_BatteryVoltage_LEN, OBC2_BatteryVoltage_CONV_D)<<endl;
			cout<<"Work mode code: "<<msg.readSignalBE(OBC2_SystemStatus_LSB, OBC2_SystemStatus_LEN)<<endl;
			cout<<"Error code: "<<msg.readSignalBE(OBC2_ErrorFlag_LSB, OBC2_ErrorFlag_LEN)<<endl;
			break;
		case OBC3:
			cout<<"AC current input: "<<msg.readSignalBE(OBC3_ACCurrentInput_LSB, OBC3_ACCurrentInput_LEN, OBC3_ACCurrentInput_CONV_D)<<endl;
			cout<<"Charging point max current: "<<msg.readSignalBE(OBC3_ChargingPileMaxCurrent_LSB, OBC3_ChargingPileMaxCurrent_LSB, OBC3_ChargingPileMaxCurrent_CONV_D)<<endl;
			cout<<"Charge port temperature 1: "<<msg.readSignalBE(OBC3_ChargePortTemp1_LSB, OBC3_ChargePortTemp1_LEN, 1, OBC3_ChargePortTemp1_OFFSET)<<endl;
			cout<<"Charge port temperature 2: "<<msg.readSignalBE(OBC3_ChargePortTemp2_LSB, OBC3_ChargePortTemp2_LEN, 1, OBC3_ChargePortTemp2_OFFSET)<<endl;
			cout<<"CP duty cycle: "<<msg.readSignalBE(OBC3_CPDutyCycle_LSB, OBC3_CPDutyCycle_LEN)<<endl;
			cout<<"Lock status (0=locked): "<<msg.readSignalBE(OBC3_LockStatus_LSB, OBC3_LockStatus_LEN)<<endl;
			cout<<"S2 status (0=disconnected): "<<msg.readSignalBE(OBC3_S2Status_LSB, OBC3_S2Status_LEN)<<endl;
			cout<<"BMS status (0=off): "<<msg.readSignalBE(OBC3_BMS_WakeStatus_LSB, OBC3_BMS_WakeStatus_LEN)<<endl;
			cout<<"12V low warning (0=normal): "<<msg.readSignalBE(OBC3_Low12VStatus_LSB, OBC3_Low12VStatus_LEN)<<endl;
			cout<<"Out relay status (0=disconnected): "<<msg.readSignalBE(OBC3_OutRelayStatus_LSB, OBC3_OutRelayStatus_LEN)<<endl;
			cout<<"CC resistance (0=disconnected): "<<msg.readSignalBE(OBC3_CCStatus_LSB, OBC3_CCStatus_LEN)<<endl;
			break;
		default:
			cout<<"Unknown message ID"<<endl;
			break;
	}
}

// Read a on/off pin
getSwitchPos(int pin) {
	int val = digitalRead(pin);
	if(val<100) {
		return 0;
	} else {
		return 1;
	}
}


int main(void) {
	// Set pins
	pinMode(PIN_ON_OFF, INPUT);
	pinMode(PIN_POTI_1, INPUT);
	pinMode(PIN_POTI_2, INPUT);
	
	// Start the CAN bus communication
	cout<<"Starting the MCP2515"<<endl;
	while(CAN_OK != CAN.begin(CAN_500KBPS)) {}	// Setting baud rate to 500Kbps
	// Create variables to store the last CAN message
	uint32_t id;
	uint8_t len;
	uint8_t frame[8];
	cout<<"Done."<<endl;
	
	// Check the bus in an infinite loop
	while(1) {
		if(CAN_MSGAVAIL == CAN.checkReceive()) {//check if data coming
			CAN.readMsgBuf(&len, frame);		// Read data length and buffer
			id = CAN.getCanId();				// Get message ID
			
			// Print if something new appears
			for(int msg=0; msg<N_MSG; msg++) {
				if(msgs[msg].getId()==id) {
					for(int i=0; i<min(len, LSCF); i++) {
						if(msgs[msg].readByte(i)!=frame[i]) {
							printMessage(id, len, frame);
							// Update the msgs array
							for(int j=0; j<len; j++) {
								msgs[msg].writeSignal(8*j, 8, frame[j]);
							}
						}
					}
				}
			}
		}
		
		// Update VCU signal
		
		
		// Write and send the VCU signal
		vcu1.send();
	}
	return 0;
}
