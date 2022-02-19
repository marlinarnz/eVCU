/* ============================== test CAN messages ======================= 
 * Tests the OBC messages' validity and prints them out using the VCU's
 * constants and CanMessage class.
 */

#include "constants.h"
#include "CanMessage.h"
#include <SPI.h>
#include <mcp2515.h>		// MCP2515 driver

// Set up the CAN interface
MCP2515 CAN(10);			// Set SPI Chip Select

// Create variables to store the last CAN message
struct can_frame frame;

// Interactive pins
const int PIN_ON_OFF = 23;
const int PIN_POTI_1 = 6;
const int PIN_POTI_2 = 9;

// Number of messages to print
const int N_MSG = 3;
// Message array
CanMessage msgs[N_MSG];

// Instantiate messages which the OBC reacts to
CanMessage msgToOBC(BMS1, &CAN, BMS1_INTERVAL);

// Interrupt handler
bool interrupt = false;
void irqHandler() {
  interrupt = true;
}


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
		case OBC1:
			Serial.println("Voltage: " + String(msg.readSignalBE(OBC1_ChargerVoltage_LSB, OBC1_ChargerVoltage_LEN, OBC1_ChargerVoltage_CONV_D)));
			Serial.println("Current: " + String(msg.readSignalBE(OBC1_ChargerCurrent_LSB, OBC1_ChargerCurrent_LEN, OBC1_ChargerCurrent_CONV_D)));
			Serial.println("Hardware status (0=OK): " + String(msg.readSignalBE(OBC1_HardwareStatus_LSB, OBC1_HardwareStatus_LEN)));
			Serial.println("Temperature status (0=OK): " + String(msg.readSignalBE(OBC1_TempAnomaly_LSB, OBC1_TempAnomaly_LEN)));
			Serial.println("AC Voltage status (0=OK): " + String(msg.readSignalBE(OBC1_ACVoltageAnomaly_LSB, OBC1_ACVoltageAnomaly_LEN)));
			Serial.println("Start status (0=OK): " + String(msg.readSignalBE(OBC1_StartStatus_LSB, OBC1_StartStatus_LEN)));
			Serial.println("Communication overtime (0=OK): " + String(msg.readSignalBE(OBC1_ComOvertime_LSB, OBC1_ComOvertime_LEN)));
			Serial.println("Battery connection (0=OK): " + String(msg.readSignalBE(OBC1_BatteryConnectStatus_LSB, OBC1_BatteryConnectStatus_LEN)));
			Serial.println("CC status (0=OK): " + String(msg.readSignalBE(OBC1_CCStatus_LSB, OBC1_CCStatus_LEN)));
			Serial.println("CP status (0=OK): " + String(msg.readSignalBE(OBC1_CPStatus_LSB, OBC1_CPStatus_LEN)));
			Serial.println("Temperature: " + String(msg.readSignalBE(OBC1_Temperature_LSB, OBC1_Temperature_LEN, 1, OBC1_Temperature_OFFSET)));
			Serial.println("Software version: " + String(msg.readSignalBE(OBC1_SoftwareVersion_LSB, OBC1_SoftwareVersion_LEN)));
			Serial.println("Hardware version: " + String(msg.readSignalBE(OBC1_HardwareVersion_LSB, OBC1_HardwareVersion_LEN)));
			break;
		case OBC2:
			Serial.println("AC voltage input: " + String(msg.readSignalBE(OBC2_ACVoltageInput_LSB, OBC2_ACVoltageInput_LEN, OBC2_ACVoltageInput_CONV_D)));
			Serial.println("PFC voltage: " + String(msg.readSignalBE(OBC2_PFCVoltage_LSB, OBC2_PFCVoltage_LEN, OBC2_PFCVoltage_CONV_D)));
			Serial.println("Current limit: " + String(msg.readSignalBE(OBC2_CurrentLimit_LSB, OBC2_CurrentLimit_LEN, OBC2_CurrentLimit_CONV_D)));
			Serial.println("Battery voltage: " + String(msg.readSignalBE(OBC2_BatteryVoltage_LSB, OBC2_BatteryVoltage_LEN, OBC2_BatteryVoltage_CONV_D)));
			Serial.println("Work mode code: " + String(msg.readSignalBE(OBC2_SystemStatus_LSB, OBC2_SystemStatus_LEN)));
			Serial.println("Error code: " + String(msg.readSignalBE(OBC2_ErrorFlag_LSB, OBC2_ErrorFlag_LEN)));
			break;
		case OBC3:
			Serial.println("AC current input: " + String(msg.readSignalBE(OBC3_ACCurrentInput_LSB, OBC3_ACCurrentInput_LEN, OBC3_ACCurrentInput_CONV_D)));
			Serial.println("Charging point max current: " + String(msg.readSignalBE(OBC3_ChargingPileMaxCurrent_LSB, OBC3_ChargingPileMaxCurrent_LSB, OBC3_ChargingPileMaxCurrent_CONV_D)));
			Serial.println("Charge port temperature 1: " + String(msg.readSignalBE(OBC3_ChargePortTemp1_LSB, OBC3_ChargePortTemp1_LEN, 1, OBC3_ChargePortTemp1_OFFSET)));
			Serial.println("Charge port temperature 2: " + String(msg.readSignalBE(OBC3_ChargePortTemp2_LSB, OBC3_ChargePortTemp2_LEN, 1, OBC3_ChargePortTemp2_OFFSET)));
			Serial.println("CP duty cycle: " + String(msg.readSignalBE(OBC3_CPDutyCycle_LSB, OBC3_CPDutyCycle_LEN)));
			Serial.println("Lock status (0=locked): " + String(msg.readSignalBE(OBC3_LockStatus_LSB, OBC3_LockStatus_LEN)));
			Serial.println("S2 status (0=disconnected): " + String(msg.readSignalBE(OBC3_S2Status_LSB, OBC3_S2Status_LEN)));
			Serial.println("BMS status (0=off): " + String(msg.readSignalBE(OBC3_BMS_WakeStatus_LSB, OBC3_BMS_WakeStatus_LEN)));
			Serial.println("12V low warning (0=normal): " + String(msg.readSignalBE(OBC3_Low12VStatus_LSB, OBC3_Low12VStatus_LEN)));
			Serial.println("Out relay status (0=disconnected): " + String(msg.readSignalBE(OBC3_OutRelayStatus_LSB, OBC3_OutRelayStatus_LEN)));
			Serial.println("CC resistance (0=disconnected): " + String(msg.readSignalBE(OBC3_CCStatus_LSB, OBC3_CCStatus_LEN)));
			break;
		default:
			Serial.println("Unknown message ID");
			break;
	}
}

// Read a on/off pin
int getSwitchPos(int pin) {
	int val = digitalRead(pin);
	return val==0;
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
  pinMode(PIN_POTI_1, INPUT);
  pinMode(PIN_POTI_2, INPUT);

  Serial.begin(115200);
  SPI.begin();

  // Fill the messages array
  msgs[0] = {OBC1, &CAN, OBC1_INTERVAL};
  msgs[1] = {OBC2, &CAN, OBC2_INTERVAL};
  msgs[2] = {OBC3, &CAN, OBC3_INTERVAL};

  // Set initial values to the BMS frame
  msgToOBC.writeSignal(BMS1_OBC_ChargeCommand_LSB,
    BMS1_OBC_ChargeCommand_LEN,
    BMS1_OBC_ChargeCommand_ON);
  msgToOBC.writeSignal(BMS1_OBC_VoltageSet_LSB,
    BMS1_OBC_VoltageSet_LEN,
    350,
    BMS1_OBC_VoltageSet_CONV_D);
  msgToOBC.writeSignal(BMS1_OBC_CurrentSet_LSB,
    BMS1_OBC_CurrentSet_LEN,
    5,
    BMS1_OBC_CurrentSet_CONV_D);

  // Init the interrupt handler
  attachInterrupt(0, irqHandler, FALLING);
	
	// Start the CAN bus communication
	Serial.println("Starting the MCP2515");
	CAN.reset();
  CAN.setBitrate(CAN_250KBPS, MCP_8MHZ);
  CAN.setNormalMode();
	Serial.println("Done.");
}
	
// Check the bus in an infinite loop
void loop() {
  if (interrupt) {//CAN.checkReceive()) {
    interrupt = false;
    uint8_t irq = CAN.getInterrupts();
    if (irq & MCP2515::CANINTF_RX1IF) {
    
      Serial.print("Message received: ");
      MCP2515::ERROR err = CAN.readMessage(MCP2515::RXB1, &frame);
      
      if (err != MCP2515::ERROR_NOMSG && err != MCP2515::ERROR_OK) {
        Serial.println("CAN bus error: " + String(err));
        delay(400);
      }
      
    	if(MCP2515::ERROR_OK == err) {
        Serial.println(String(frame.can_id, HEX));
        //printMessage(frame.can_id, frame.can_dlc, frame.data);
    		// Print if something new appears
    		/*for(int msg=0; msg<N_MSG; msg++) {
    			if(msgs[msg].getId()==frame.can_id) {
    				for(int i=0; i<min(frame.can_dlc, LSCF); i++) {
    					if(msgs[msg].readByte(i)!=frame.data[i]) {
                printMessage(frame.can_id, frame.can_dlc, frame.data);
    				    // Update the msgs array
    						for(int j=0; j<frame.can_dlc; j++) {
    							msgs[msg].writeByte(j, frame.data[j]);
    						}
    					}
    				}
    			}
    		}*/
    	}
    }
  }
	
	// Update BMS signal
	/*msgToOBC.writeSignal(BMS1_OBC_ChargeCommand_LSB,
		BMS1_OBC_ChargeCommand_LEN,
		getSwitchPos(PIN_ON_OFF));
  int volt = getPotiPos(PIN_POTI_1, 0, 350);
  msgToOBC.writeSignal(BMS1_OBC_VoltageSet_LSB,
    BMS1_OBC_VoltageSet_LEN,
    volt,
    BMS1_OBC_VoltageSet_CONV_D);
  int amp = getPotiPos(PIN_POTI_2, 0, 5);
  msgToOBC.writeSignal(BMS1_OBC_CurrentSet_LSB,
    BMS1_OBC_CurrentSet_LEN,
    amp,
    BMS1_OBC_CurrentSet_CONV_D);*/
	
	// Write and send the BMS signal
	msgToOBC.send();
  uint8_t data[8];
  for(int i=0; i<8; i++) {
    data[i] = {(uint8_t)(msgToOBC.readByte(i))};
  }
  //printMessage(msgToOBC.getId(), 8, data);
}
