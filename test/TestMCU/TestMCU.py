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
logfile = open('logMCU.log', 'w')
logger = can.CanutilsLogWriter(logfile, channel='can0')

# Listen for a while
print('Listen to the bus...')
t0 = time.time()
t_send = time.time()
frame = [0x0,0x0,0x0,0x20,0x09,0x84,0x0,0x0]
while time.time() - t0 < 60:
	msg = can0.recv(1)
	if msg is not None:
		logger.on_message_received(msg)
		print(msg)
	else:
		print('No messages')
	if time.time() - t_send > 0.02:
		# Send VCU message
		# Key position crank
		if GPIO.input(26) == GPIO.HIGH:
			frame[5] = 0xc4
		else:
			frame[5] = 0x84
		# Torque
		if GPIO.input(19) == GPIO.HIGH:
			frame[0] = 0x06
		else:
			frame[0] = 0
		# Rolling counter
		frame[6] = frame[6] + 1
		if frame[6] > 0xf:
			frame[6] = 0
		# Check sum
		frame[7] = sum(frame[:7]) ^ 0xff
		try:
			vcu_msg = can.Message(arbitration_id=0x101, data=frame, extended_id=False)
			can0.send(vcu_msg)
			logger.on_message_received(vcu_msg)
			print(vcu_msg)
		except can.CanError:
			print('Message send error')
		t_send = time.time()


# Terminate the bus
logfile.close()
os.system('sudo ifconfig can0 down')
