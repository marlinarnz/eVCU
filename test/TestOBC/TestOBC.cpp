/* ============================== test CAN messages ======================= 
 * Tests the OBC messages' validity and prints them out using the VCU's
 * constants and CanMessage class.
 */

#include "TestOBC.h"
#include "constants.h"
#include "CanMessage.h"
#include <wiringPi.h>		// Library to use C code on the Pi's GPIO pins
#include <mcp_can.h>		// MCP2515 driver
#include <iostream>			// Command line interaction

using namespace std;

// Set up the CAN interface
// https://www.raspberrypi.org/documentation/usage/gpio/
// https://www.raspberrypi.org/forums/viewtopic.php?f=44&t=141052
wiringPiSetupGpio();
MCP_CAN CAN(24);			// Set SPI Chip Select

// Instantiate messages which the OBC reacts to
CanMessage msgToOBC(BMS1, CAN, BMS1_INTERVAL);
msgToOBC.writeSignal(BMS1_OBC_ChargeCommand_LSB,
	BMS1_OBC_ChargeCommand_LEN,
	BMS1_OBC_ChargeCommand_OFF);


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


int main(void) {
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
			printMessage(id, len, frame);
		}
		
		// Write and send the BMS signal
		msgToOBC.send();
	}
	return 0;
}
