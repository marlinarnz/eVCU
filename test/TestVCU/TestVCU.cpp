/* ============================== test CAN messages ======================= 
 * Tests the VCU messages' validity and prints them out using the VCU's
 * constants and CanMessage class.
 */

#include "TestVCU.h"
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
		case VCU1:
			cout<<"Torque request (%): "<<msg.readSignalBE(VCU1_TorqueReq_LSB, VCU1_TorqueReq_LEN, VCU1_TorqueReq_CONV_D)<<endl;
			cout<<"Work mode (0=torque): "<<msg.readSignalBE(VCU1_WorkMode_LSB, VCU1_WorkMode_LEN)<<endl;
			cout<<"Motor mode (1=drive, 2=generate): "<<msg.readSignalBE(VCU1_MotorMode_LSB, VCU1_MotorMode_LEN)<<endl;
			cout<<"Brake pedal status: "<<msg.readSignalBE(VCU1_BrakePedalSts_LSB, VCU1_BrakePedalSts_LEN)<<endl;
			cout<<"Gear lever position (1=reverse): "<<msg.readSignalBE(VCU1_GearLeverPosSts_LSB, VCU1_GearLeverPosSts_LEN)<<endl;
			cout<<"Main relay: "<<msg.readSignalBE(VCU1_BMS_MainRelayCmd_LSB, VCU1_BMS_MainRelayCmd_LEN)<<endl;
			cout<<"Aux relay: "<<msg.readSignalBE(VCU1_BMS_AuxRelayCmd_LSB, VCU1_BMS_AuxRelayCmd_LEN)<<endl;
			cout<<"Warning level: "<<msg.readSignalBE(VCU1_WarningLevel_LSB, VCU1_WarningLevel_LEN)<<endl;
			cout<<"Key position: "<<msg.readSignalBE(VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN)<<endl;
			
			cout<<"Unused signals:"<<endl;
			cout<<"Speed request (rpm): "<<msg.readSignalBE(VCU1_MotorSpdReq_LSB, VCU1_MotorSpdReq_LEN, VCU1_MotorSpdReq_CONV_D)<<endl;
			cout<<"Reserved: "<<msg.readSignalBE(VCU1_Reserved_LSB, VCU1_Reserved_LEN)<<endl;
			cout<<"Authentication status: "<<msg.readSignalBE(VCU1_AuthenticationStatus_LSB, VCU1_AuthenticationStatus_LEN)<<endl;
			cout<<"Change gear alarm: "<<msg.readSignalBE(VCU1_ChangeGearAlarm_LSB, VCU1_ChangeGearAlarm_LEN)<<endl;
			cout<<"Vehicle state: "<<msg.readSignalBE(VCU1_VehicleState_LSB, VCU1_VehicleState_LEN)<<endl;
			cout<<"Gear lever position fail: "<<msg.readSignalBE(VCU1_GearLeverPosSts_F_LSB, VCU1_GearLeverPosSts_F_LEN)<<endl;
			cout<<"AC control: "<<msg.readSignalBE(VCU1_AC_ControlCmd_LSB, VCU1_AC_ControlCmd_LEN)<<endl;
			cout<<"AC power reduce request: "<<msg.readSignalBE(VCU1_AC_PowerReduceReq_LSB, VCU1_AC_PowerReduceReq_LEN)<<endl;
			cout<<"Check sum: "<<msg.readSignalBE(VCU1_CheckSum_LSB, VCU1_CheckSum_LEN)<<endl;
			cout<<"Rolling counter: "<<msg.readSignalBE(VCU1_RollingCounter_LSB, VCU1_RollingCounter_LEN)<<endl;
			break;
		case VCU2:
			cout<<"Urgency power cutoff: "<<msg.readSignalBE(VCU2_UrgencyCutOffPowerReq_LSB, VCU2_UrgencyCutOffPowerReq_LEN)<<endl;
			
			cout<<"Unused signals:"<<endl;
			cout<<"Charge enable: "<<msg.readSignalBE(VCU2_ChargeEnable_LSB, VCU2_ChargeEnable_LEN)<<endl;
			cout<<"BMS heating command: "<<msg.readSignalBE(VCU2_BMS_HeatingCmd_LSB, VCU2_BMS_HeatingCmd_LEN)<<endl;
			cout<<"AC maximum power: "<<msg.readSignalBE(VCU2_AC_MaxPowerLimit_LSB, VCU2_AC_MaxPowerLimit_LEN)<<endl;
			cout<<"Check sum: "<<msg.readSignalBE(VCU1_CheckSum_LSB, VCU1_CheckSum_LEN)<<endl;
			cout<<"Rolling counter: "<<msg.readSignalBE(VCU1_RollingCounter_LSB, VCU1_RollingCounter_LEN)<<endl;
			break;
		case VCU3:
			cout<<"Park brake: "<<msg.readSignalBE(VCU3_ParkBrake_LSB, VCU3_ParkBrake_LEN)<<endl;
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
	
	// Check the messages in an infinite loop
	// You should increase the send intervals in the constants to
	// have a better grasp of what happens
	while(1) {
		if(CAN_MSGAVAIL == CAN.checkReceive()) {//check if data coming
			CAN.readMsgBuf(&len, frame);		// Read data length and buffer
			id = CAN.getCanId();				// Get message ID
			printMessage(id, len, frame);
		}
	}
	return 0;
}
