/* ============================== References ==============================
 *  Author: Marlin Arnz
 *  This code includes the basic functionality to control and configure
 *  an electric vehicle via CAN-bus with an Arduino as vehicle control
 *  unit (VCU). It includes all necessary elements motor and inbuilt gauge 
 *  operation, whereas advanced user interface, battery management and
 *  on-board charging systems are outsourced.
 *  The VCU is intended to start anew each time the key is turned on.
 *  One must implement a reset function for continuous operation.
 *  
 *  Version date: 2020-27-04
 *  Version: 0.1
 *  Created date: 2020-27-04
 *  
 *  License: MIT
 *  
 * Copyright 2020 Marlin Arnz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/* ============================== Libraries =============================== */
#include <SPI.h>              // Communication via SPI
#include <mcp2515.h>          // Lib for MCP2515 CAN communication
#include <EEPROM.h>           // EEPROM storage access
#include "PowerButton.h"      // Class of the start/stop button
#include "constants.h"        // Values for car operation
#include "driveUtils.h"       // Utility functions for driving
#include "Throttle.h"         // Throttle class
#include "commUtils.h"        // Utility functions for communication
#include "CanManager.h"       // CAN bus communication manager


/* ============================== Variables and objects =================== */
bool on = false;              // State of the MCU (motor controller)
bool recu;                    // State of the brake energy recuperation switch
bool direc;                   // State of the direction switch
                              // true = forward; false = backwards

// Time variables in milliseconds
unsigned long startTime;
unsigned long prechargeStart;
unsigned long lastLoop;

// Instantiate an MCP2515 driver and give it to the CAN bus manager
MCP2515 CAN(selectCANPin);    // Set SPI Chip Select
CanManager can(&CAN);

// Instantiate a throttle object
Throttle throttle(&can);

// Instantiate a power button object
PowerButton powerButton(&can);


/* ============================== Setup =================================== */
void setup() {
  Serial.begin(9600);
  
  pinMode(direcPin, INPUT);
  pinMode(recuPin, INPUT);
  pinMode(handBrakePin, INPUT);
  pinMode(driverDoorPin, INPUT);
  pinMode(prechargeRelaisPin, OUTPUT);
  pinMode(mainContactorPin, OUTPUT);
  pinMode(kl15Pin, OUTPUT);
  pinMode(soundPin, OUTPUT);
  pinMode(selectCANPin, OUTPUT);
  pinMode(engineFaultLightPin, OUTPUT);
  pinMode(coolantTempGaugePin, OUTPUT);
  pinMode(fuelGaugePin, OUTPUT);
  
  pinMode(powerButtonLEDPin, OUTPUT);
  digitalWrite(powerButtonLEDPin, LOW);  // PowerButton light off

  //resetEEPROM();              // update EEPROM values to default (see constants)

  can.begin();                // Start the CAN bus communication

  throttle.begin(throttlePin, brakePedalPin);

  initMCU(PRCH);              // Initialise the MCU
  powerButton.begin(powerButtonPin, powerButtonLEDPin); // Init PowerButton
  
  startTime = millis();       // Save setup finish time
}

/* ============================== Main loop  ============================== */
void loop() {
  can.update();               // Read news, spread news, save news
  bool running = getMotorState() == MCU1_MotorMainState_RUNNING;
  if (running && !on) {
    throttle.reset();         // Reset the throttle if motor gets started
  } else if (!running && on) {
    powerButton.stopMotor();  // Update power button if motor stopped itself
  }
  on = running;               // Read the motor status
  powerButton.update();       // Read if start/stop button was pressed
  updateWarnSignals();        // Manage all the warn signals
  if (on) {
    updateDirec();            // Read state of the direction switch
    updateRecu();             // Read state of the recuperation switch
    throttle.update();        // Accelerate, brake and cruise
    cool(int(can.readSignal(MCU2, MCU2_MotorTemp_LSB, MCU2_MotorTemp_LEN, 1, MCU2_MotorTemp_OFFSET)),
         int(can.readSignal(MCU2, MCU2_HardwareTemp_LSB, MCU2_HardwareTemp_LEN, 1, MCU2_HardwareTemp_OFFSET))
         );                   // Check the temps and chill down if needed
  }
  updateGauges();             // Update all builtin gauges
}


/* ============================== MCU initialisation ======================
 * Starts the motor controller and checks its state for faults
 * @param precharge: boolean whether to open the precharge relais or the
 *                   main contactor
 */
void initMCU(bool precharge) {
  digitalWrite(kl15Pin, HIGH);
  can.writeSignal(VCU1, VCU1_KeyPosition_LSB, VCU1_KeyPosition_LEN, VCU1_KeyPosition_ACC);
  if (precharge) {
    digitalWrite(prechargeRelaisPin, HIGH); // Switch on the precharge relais
    prechargeStart = millis();              // to let current rush the MCU caps
    can.writeSignal(VCU1, VCU1_BMS_AuxRelayCmd_LSB, VCU1_BMS_AuxRelayCmd_LEN, VCU1_BMS_AuxRelayCmd_ON);
  } else {
    digitalWrite(mainContactorPin, HIGH);
    can.writeSignal(VCU1, VCU1_BMS_MainRelayCmd_LSB, VCU1_BMS_MainRelayCmd_LEN, VCU1_BMS_MainRelayCmd_ON);
  }

  bool MCUready = false;
  long endTime = millis() + MITL;

  //TODO check MITL too short or too long
  
  while(!MCUready && millis() < endTime) {
    int state = getMotorState();
    MCUready = can.checkMCUready(true)
               && (state == MCU1_MotorMainState_STANDBY
                  || state == MCU1_MotorMainState_PRECHARGE
                  || state == MCU1_MotorMainState_READY);
    delay(10);
  }
  if (MCUready) {
    report("CONTROLLER_INIT_SUCCESS", 1); //TODO
  } else {
    report("CONTROLLER_INIT_FAULT", 3); //TODO
  }
}


/* ============================== Warn signals ============================
 * Manages all the warning signals for save operation of the vehicle
 */
void updateWarnSignals() {
  
  // Manage acoustic warn signals
  if (checkDriverDoorOpen() && on) {
    tone(soundPin, TFDD);     // Start an acoustic signal
  } else if (!checkHandBrake() && !on) {
    tone(soundPin, TFHB);     // Start an acoustic signal
  } else {
    noTone(soundPin);         // End all acoustic signals on this pin
  }
  
  // Read CAN signals and check for errors or warnings from other ECUs
  // Fatal errors: Require a reset to eventually start the car again
  if (false) {
    digitalWrite(mainContactorPin, LOW);
    powerButton.stopMotor();
    setWarningLevel(3);
    can.writeSignal(VCU2, VCU2_UrgencyCutOffPowerReq_LSB, VCU2_UrgencyCutOffPowerReq_LEN, VCU2_UrgencyCutOffPowerReq_ERROR);
  }
  // Motor/MCU errors
  if (!can.checkMCUready() || !can.checkBMSready()) {
    powerButton.stopMotor();
  }
  // Motor/MCU warnings
  if (can.readSignal(MCU1, MCU1_ActMotorSpd_LSB, MCU1_ActMotorSpd_LEN) == MCU1_ActMotorSpd_INVALID
      || can.readSignal(MCU1, MCU1_ActMotorTq_LSB, MCU1_ActMotorTq_LEN) == MCU1_ActMotorTq_INVALID
      || can.readSignal(MCU1, MCU1_MaxMotorTq_LSB, MCU1_MaxMotorTq_LEN) == MCU1_MaxMotorTq_INVALID
      || can.readSignal(MCU1, MCU1_MaxMotorBrakeTq_LSB, MCU1_MaxMotorBrakeTq_LEN) == MCU1_MaxMotorBrakeTq_INVALID
      || can.readSignal(MCU1, MCU1_MaxMotorTq_LSB, MCU1_MaxMotorTq_LEN) > EEPROM.read(MAX_TORQUE)
      || can.readSignal(MCU1, MCU1_MaxMotorBrakeTq_LSB, MCU1_MaxMotorBrakeTq_LEN) > EEPROM.read(MAX_NEG_TORQUE)
      || can.readSignal(MCU1, MCU1_MotorRatoteDirection_LSB, MCU1_MotorRatoteDirection_LEN) == MCU1_MotorRatoteDirection_ERROR
      || can.readSignal(MCU2, MCU2_MotorTemp_LSB, MCU2_MotorTemp_LEN) == MCU2_MotorTemp_INVALID
      || can.readSignal(MCU2, MCU2_HardwareTemp_LSB, MCU2_HardwareTemp_LEN) == MCU2_HardwareTemp_INVALID
      || can.readSignal(MCU2, MCU2_PhaseCurrSensorState_LSB, MCU2_PhaseCurrSensorState_LEN) == MCU2_PhaseCurrSensorState_ERROR
      || can.readSignal(MCU2, MCU2_MotorSensorState_LSB, MCU2_MotorSensorState_LEN) == MCU2_MotorSensorState_ERROR
      || can.readSignal(MCU2, MCU2_DC_VoltSensorState_LSB, MCU2_DC_VoltSensorState_LEN) == MCU2_DC_VoltSensorState_ERROR
      || can.readSignal(MCU2, MCU2_DC_LowVoltWarning_LSB, MCU2_DC_LowVoltWarning_LEN) == MCU2_DC_LowVoltWarning_ERROR
      || can.readSignal(MCU2, MCU2_12V_LowVoltWarning_LSB, MCU2_12V_LowVoltWarning_LEN) == MCU2_12V_LowVoltWarning_ERROR
      || can.readSignal(MCU2, MCU2_WarningLevel_LSB, MCU2_WarningLevel_LEN) >= MCU2_WarningLevel_ERROR1
      || can.readSignal(MCU2, MCU2_MotorStallFault_LSB, MCU2_MotorStallFault_LEN) == MCU2_MotorStallFault_ERROR
      || can.readSignal(MCU3, MCU3_DC_MainWireVolt_LSB, MCU3_DC_MainWireVolt_LEN) == MCU3_DC_MainWireVolt_INVALID
      || can.readSignal(MCU3, MCU3_DC_MainWireCurr_LSB, MCU3_DC_MainWireCurr_LEN) == MCU3_DC_MainWireCurr_INVALID
      || can.readSignal(MCU3, MCU3_MotorPhaseCurr_LSB, MCU3_MotorPhaseCurr_LEN) == MCU3_MotorPhaseCurr_INVALID
      ) {
    
    //TODO: Add BMS warnings in the if condition
    
    digitalWrite(engineFaultLightPin, HIGH);
  } else {
    digitalWrite(engineFaultLightPin, LOW);
  }
  // Brake light
  if (can.readSignal(MCU1, MCU1_ActMotorTq_LSB, MCU1_ActMotorTq_LEN, MCU1_ActMotorTq_CONV_D) >= EEPROM.read(BRAKE_LIGHT_TORQUE_THRESHOLD)
      && can.readSignal(MCU1, MCU1_MotorState_LSB, MCU1_MotorState_LEN) == MCU1_MotorState_GENERATE) {
    digitalWrite(brakeLightPin, HIGH);
  } else {
    digitalWrite(brakeLightPin, LOW);
  }
}


/* ============================== Gauges ==================================
 * Manages the gauges of the car
 */
void updateGauges() {
  // Temperature gauge
  int motorTemp = int(can.readSignal(MCU2, MCU2_MotorTemp_LSB, MCU2_MotorTemp_LEN, 1, MCU2_MotorTemp_OFFSET));
  analogWrite(coolantTempGaugePin,
              int(max(MTGL, min(MTGU, motorTemp)) * (MTGU / 255)));
  
  // Fuel gauge --> read BMS
  //TODO
}


/* ============================== MCU status ==============================
 * Returns the motor state
 * @return: int motor state (see constants)
 */
int getMotorState() {
  
  //TODO what does MotorState/MotorMainState_STANDBY and READY mean?

  int state = int(can.readSignal(MCU1, MCU1_MotorMainState_LSB, MCU1_MotorMainState_LEN));
  if (state == MCU1_MotorMainState_RUNNING
      && ((can.readSignal(MCU1, MCU1_MotorRatoteDirection_LSB, MCU1_MotorRatoteDirection_LEN) == MCU1_MotorRatoteDirection_FORWARD && !direc)
          || (can.readSignal(MCU1, MCU1_MotorRatoteDirection_LSB, MCU1_MotorRatoteDirection_LEN) == MCU1_MotorRatoteDirection_BACKWARD && direc))) {
    report("MOTOR_WRONG_DIRECTION", 3);
  }
  return state;
}


/* ============================== Driver door open ========================
 * Returns true or false when driver door is open or closed, respectively.
 * Necessary for the German TÜV.
 * NOTE: Pin is high when door is open
 * @return: bool true if door is open, false otherwise
 */
bool checkDriverDoorOpen() {
  return (digitalRead(driverDoorPin) == HIGH);
}


/* ============================== Hand brake ==============================
 * Returns true or false for tightened or loose park brake, respectively.
 * NOTE: Pin is high when hand brake tightened
 * @return: bool true if brake is tightened, false otherwise
 */
bool checkHandBrake() {
  bool tight = (digitalRead(handBrakePin) == HIGH);
  can.writeSignal(VCU3, VCU3_ParkBrake_LSB, VCU3_ParkBrake_LEN, !tight);
  return tight;
}
