import os
import time
import can
import binascii
import RPi.GPIO as GPIO

import tkinter as tk

import can_decoder as cd


class MCUTestGUI(tk.Frame):
	# Testing interface
	
	def __init__(self, logger=True, name='APEV528 test terminal'):
		# Init pins
		GPIO.setwarnings(False)
		GPIO.setmode(GPIO.BCM)
		GPIO.setup(26, GPIO.IN) # Pushbutton: key position crank
		GPIO.setup(19, GPIO.IN) # Lever switch: torque
		GPIO.setup(13, GPIO.IN) # Standard switch: Key on/acc
		GPIO.setup(2, GPIO.OUT) # Relay
		GPIO.setup(3, GPIO.OUT) # Relay
		GPIO.setup(4, GPIO.OUT) # Relay
		GPIO.output(2, True) # Relay switches with negative input
		GPIO.output(3, True)
		GPIO.output(4, True)
		self.gear_pos = 0b01000000 # Park
		self.key_pos = 0b01000000 # Acc
		
		# CAN utils
		self.__can0 = None
		self.send_time = 0

		# Create a log file?
		self.log = logger
		self.logfile = None
		self.logger = None

		# Decoder
		self.decoder = cd.Decoder(False, 8)

		# Define the top level GUI frame
		self.__top = tk.Tk()
		self.__top.title(name)
		self.__top.geometry("1010x500")
		cells = 0
		grid_frame = 10
		while cells < grid_frame:
			self.__top.rowconfigure(cells, weight=1)
			self.__top.columnconfigure(cells, weight=1)
			cells += 1

		# Add a start/stop button
		self.__on = False
		def ss():
			if self.__on:
				self.stop()
			else:
				try:
					self.start()
				except Exception as e:
					print(e)
					self.stop()
		self.__button_run = tk.Button(self.__top, text='Start/Stop Comm', command=ss)
		self.__button_run.grid(row=1, column=1, columnspan=3)
		
		self.__running = False
		'''# Add a HV connect button
		def hv():
			if self.__on and not self.__running:
				self.connect_hv()
				self.__running = True
			elif self.__on and self.__running:
                                self.disconnect_hv()
                                self.__running = False
		self.__button_hv = tk.Button(self.__top, text='HV/Motor start/stop', command=hv)
		self.__button_hv.grid(row=1, column=5, columnspan=3)'''

		# Add message streams
		messages = [0x101, 0x105, 0x106, 0x107]
		self.__streams = {hex(id): tk.StringVar() for id in messages}
		width = int((grid_frame-0)/len(messages))
		self.__stream_labels = {id: tk.Label(self.__top, text=str(id)).grid(
			row=2, column=0 + i*width, columnspan=width, sticky='W')
			for id, i in zip(self.__streams.keys(), range(len(messages)))}
		self.__stream_boxes = {id: tk.Label(self.__top, textvariable=var).grid(
			row=3, column=0 + i*width, rowspan=grid_frame-3, columnspan=width, sticky='NE')
			for id, var, i in zip(self.__streams.keys(), self.__streams.values(), range(len(messages)))}
		# Init stream text
		for id, var in self.__streams.items():
			var.set(self.decoder.decode(id[2:], [0]*8, sep='\n'))

	def run(self):
		# Run the GUI
		while True:
			self.loop()

	def loop(self):
		try:
			self.__top.update_idletasks()
			self.__top.update()
			self.read()
			self.write()
			# Key switches start or stop the motor
			if not self.__running and GPIO.input(13) == GPIO.HIGH:
				self.connect_hv()
			elif self.__running and GPIO.input(13) == GPIO.LOW:
				self.disconnect_hv()
		except:
			if self.__on:
				print('An error occured')
				self.stop()

	def log_can_msg(self, msg):
		# Log the CAN bus message
		if self.log:
			self.logger.on_message_received(msg)
			#self.decoder.decode_message(str(msg)))

	def start(self):
		self.__on = True
		# Init CAN
		os.system('sudo ip link set can0 type can bitrate 500000')
		os.system('sudo ifconfig can0 up')
		# Bind socket
		self.__can0 = can.interface.Bus(channel='can0', bustype='socketcan_ctypes')

		if self.log:
			# Start the logger
			self.logfile = open('logMCU_OEM.log', 'w')
			self.logger = can.CanutilsLogWriter(self.logfile, channel='can0')

		self.vcu_frame = [0, # Torque Req / 0.392
			0, # Speed Req / 0.25
			0, # continued
			0b00100000,# Change gear and authentication, irrelevant for MCU
			0b00001001, # with gear D: 0b00110000, R: 0b00010000
			0b10000100, # key pos., motor mode (bit1), warning level
			0x0,
			0x0]
		self.bms_frame = [0, # Warning level, status, relays
			192, # SOC / 0.5, irrelevant
			198, # SOH / 0.5, irrelevant
			0b01000000, # Precharge status off, power on
			0,
			0,
			0x0,
			0x0]
		print('Switched CAN bus on')
		GPIO.output(2, False)
		print('Switched APEV528 on')

	def stop(self):
		self.__on = False
		# Switch off everything
		self.key_pos = 0b00000000
		print('Switched key position off')
		self.gear_pos = 0b01000000
		print('Switched gear lever to park')
		GPIO.output(4, True)
		GPIO.output(3, True)
		print('Switched HV off')
		GPIO.output(2, True)
		print('Switched APEV528 off')
		# Stop the bus
		os.system('sudo ifconfig can0 down')
		print('Switched CAN bus off')
		if self.log: self.logfile.close()

	def connect_hv(self):
		self.__running = True
		#self.key_pos = 0b10000000
		#print('Switched key position on')
		# Switch relays
		start = time.time()
		GPIO.output(3, False)
		self.bms_frame[3] = 0b01000100
		print('Switched precharge on')
		# Wait for precharge
		while time.time() - start < 0.5:
			self.loop()
		GPIO.output(3, True)
		GPIO.output(4, False)
		self.bms_frame[3] = 0b01010000
		print('Switched HV on')
		self.gear_pos = 0b00110000
		print('Switched gear lever to drive')

	def disconnect_hv(self):
		self.__running = False
		self.gear_pos = 0b01000000
		print('Switched gear lever to park')
		#self.key_pos = 0b01000000
		#print('Switched key position Acc')
		# Switch relays
		GPIO.output(3, True)
		GPIO.output(4, True)
		print('Switched HV off')

	def read(self):
		# Read the CAN bus
		if self.__on:
			for msg in self.__can0:
				if msg is not None:
					self.log_can_msg(msg)
					self.update_stream(msg)

	def write(self):
		# Write the VCU message on the bus
		if self.__on:
			if time.time() - self.send_time >= 0.02:
				# Gear position (and vehicle state)
				self.vcu_frame[4] = self.vcu_frame[4] | self.gear_pos
				# Key position
				if self.__running:
					if GPIO.input(26) == GPIO.HIGH:
						self.key_pos = 0b11000000 # crank
					else:
						self.key_pos = 0b10000000 # on
				else:
					self.key_pos = 0b01000000 # acc
				self.vcu_frame[5] = 0b00000100 | self.key_pos
				# Torque/speed
				if GPIO.input(19) == GPIO.HIGH and self.key_pos == 0b10000000:
					self.vcu_frame[0] = 0b00100000
				else:
					self.vcu_frame[0] = 0
				# Rolling counter (4 bit)
				self.vcu_frame[6] = self.vcu_frame[6] + 1
				if self.vcu_frame[6] > 0xf:
					self.vcu_frame[6] = 0
				self.bms_frame[6] = self.bms_frame[6] + 1
				if self.bms_frame[6] > 0xf:
					self.bms_frame[6] = 0
				# Check sum (8 bit)
				self.vcu_frame[7] = (sum(self.vcu_frame[:7]) ^ 0xff) & 0xff
				self.bms_frame[7] = (sum(self.bms_frame[:7]) ^ 0xff) & 0xff
				# Send message
				try:
					vcu_msg = can.Message(arbitration_id=0x101,
						data=self.vcu_frame, extended_id=False)
					self.__can0.send(vcu_msg)
					self.log_can_msg(vcu_msg)
					self.update_stream(vcu_msg)
				except can.CanError as e:
					print('Message send error: {}'.format(e))
				self.send_time = time.time()
				# Send other messages
				try:
					hand_brake_msg = can.Message(arbitration_id=0x431,
						data=[0,0,0,0,0,0,0,0], extended_id=False)
					self.__can0.send(hand_brake_msg)
					self.log_can_msg(hand_brake_msg)
					bms = can.Message(arbitration_id=0x1A0,
						data=self.bms_frame, extended_id=False)
					self.__can0.send(bms)
					self.log_can_msg(bms)
				except can.CanError as e:
					print('Message send error: {}'.format(e))

	def update_stream(self, msg):
		# Update message frames on GUI and console
		print(msg)
		try:
			msg = self.decoder.decode_message(str(msg), keep_time=None, sep='\n')
			if msg:
				self.__streams[msg[:5]].set(msg[7:])
		except (ValueError, KeyError) as e:
			print(getattr(e, 'message', repr(e)))


if __name__ == "__main__":
	gui = MCUTestGUI()
	gui.run()
