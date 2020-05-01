/* ============================== test VCU reactions ======================= 
 * Tests the VCU's reactions to emulated ECU signals
 */

#include "test_VCU_message.h"
#include "../../vcu/constants.h"
#include "../../vcu/CanMessage.h"
#include <wiringPi.h>		// Library to use C code on the Pi's GPIO pins
#include <mcp_can.h>		// MCP2515 driver
#include <iostream>			// Command line interaction

using namespace std;

// Set up the CAN interface
wiringPiSetupGpio();
MCP_CAN CAN(24);			// Set SPI Chip Select


int main(void) {
	// Start the CAN bus communication
	cout<<"Starting the MCP2515"<<endl;
	while(CAN_OK != CAN.begin(CAN_500KBPS)) {}	// Setting baud rate to 500Kbps
	// Create variables to store the last CAN message
	uint32_t id;
	uint8_t len;
	uint8_t frame[8];
	cout<<"Done."<<endl;
	
	return 1;
}