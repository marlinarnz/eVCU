import os
import time
import can
import binascii
import RPi.GPIO as GPIO

# Init buttons
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(13, GPIO.IN)
GPIO.setup(19, GPIO.IN)
GPIO.setup(26, GPIO.IN)

# Init CAN
os.system('sudo ip link set can0 type can bitrate 500000')
os.system('sudo ifconfig can0 up')

# Bind socket
can0 = can.interface.Bus(channel='can0', bustype='socketcan_ctypes')

# Start the logger
logfile = open('logMCU_OEM.log', 'w')
logger = can.CanutilsLogWriter(logfile, channel='can0')

# Listen for a while
print('Listen to the bus...')
t0 = time.time()
t_send = time.time()
frame = [0, # Torque Req / 0.392
	0, # Speed Req
	0, # continued
	0,# Change gear and authentication, irrelevant for MCU
	0b00001001, # with gear D: 0b00111001, R: 0b00011001
	0b10000100, # key pos., motor mode, warning level
	0x0,
	0x0]
while GPIO.input(13) == GPIO.HIGH:
	msg = can0.recv(1)
	if msg is not None:
		logger.on_message_received(msg)
		print(msg)
	else:
		print('No messages')
	if time.time() - t_send > 0.02:
		# Fill VCU message
		# Key position crank
		if GPIO.input(26) == GPIO.HIGH:
			frame[5] = 0b11000100
		else:
			frame[5] = 0b10000100
		# Torque
		if GPIO.input(19) == GPIO.HIGH:
			frame[0] = 0x11
		else:
			frame[0] = 0
		# Rolling counter
		frame[6] = frame[6] + 1
		if frame[6] > 0xf:
			frame[6] = 0
		# Check sum (8 bit)
		frame[7] = (sum(frame[:7]) ^ 0xff) & 0xff
		# Send message
		try:
			vcu_msg = can.Message(arbitration_id=0x101, data=frame, extended_id=False)
			can0.send(vcu_msg)
			logger.on_message_received(vcu_msg)
			print(vcu_msg)
		except can.CanError:
			print('Message send error: {}'.format(str(can.CanError)))
		'''# Send hand brake message
		try:
			hand_brake_msg = can.Message(arbitration_id=0x431, data=[0,0,0,0,0,0,0,0], extended_id=False)
			can0.send(hand_brake_msg)
			logger.on_message_received(hand_brake_msg)
			print(hand_brake_msg)
		except can.CanError:
			print('Message send error: {}'.format(str(can.CanError)))'''
		t_send = time.time()


# Terminate the bus
print('End bus...')
logfile.close()
os.system('sudo ifconfig can0 down')
