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

print('Listen to the bus...')
t0 = time.time()
t_send = time.time()
frame = [0x0, 0x7D, 0x68, 0x42, 0x0, 0x0, 0x80, 0x0]
step = 'waiting'

# Listen until swiched off
while GPIO.input(13) == GPIO.HIGH:
	msg = can0.recv(1) # Get MCU message
	if msg is not None:
		# Check if motor is ready
		if msg.arbitration_id == 0x105 and msg.data[6] >= 0x50 and step == 'waiting':
			t0 = time.time()
			step = 'mcu_ready'
			print(step)
		logger.on_message_received(msg)
		# Print message only if switch is on
		if GPIO.input(19) == GPIO.HIGH:
			print(msg)
	else:
		print('No messages')
	
	# Go through the test steps
	if step == 'mcu_ready':
		frame = [0x0, 0x7D, 0x68, 0x42, 0xB0, 0x36, 0x92, 0x0]
		step = '0_torque'
		print(step)
	if step == '0_torque' and time.time() - t0 > 5:
		frame = [0x52, 0x80, 0x68, 0x42, 0xB0, 0x36, 0x92, 0x0]
		step = 'forward'
		print(step)
	if step == 'forward' and time.time() - t0 > 10:
		frame = [0x0, 0x7D, 0x68, 0x42, 0xB0, 0x36, 0x92, 0x0]
		step = 'stop_forward1'
		print(step)
	if step == 'stop_forward1' and time.time() - t0 > 11:
		frame = [0x0, 0x7D, 0x68, 0x42, 0xB0, 0x36, 0x81, 0x0]
		step = 'stop_forward2'
		print(step)
	if step == 'stop_forward2' and time.time() - t0 > 14:
		frame = [0x0, 0x7D, 0x68, 0x42, 0xB0, 0x36, 0x92, 0x0]
		step = 'backward1'
		print(step)
	if step == 'stop_forward2' and time.time() - t0 > 15:
		frame = [0x52, 0x80, 0x68, 0x42, 0xB0, 0x36, 0x8A, 0x0]
		step = 'backward2'
		print(step)
	if step == 'backward2' and time.time() - t0 > 20:
		frame = [0x0, 0x7D, 0x68, 0x42, 0xB0, 0x36, 0x92, 0x0]
		step = 'stop_backward1'
		print(step)
	if step == 'stop_backward1' and time.time() - t0 > 21:
		frame = [0x0, 0x7D, 0x68, 0x42, 0xB0, 0x36, 0x81, 0x0]
		step = 'stop_backward2'
		print(step)
	if step == 'stop_backward2' and time.time() - t0 > 23:
		frame = [0x0, 0x7D, 0x68, 0x42, 0xB0, 0x36, 0xC0, 0x0]
		step = 'off'
		print(step)
	
	# Send message in intervals
	if time.time() - t_send > 0.02:
		try:
			vcu_msg = can.Message(arbitration_id=0x0C19F0A7, data=frame, extended_id=True)
			can0.send(vcu_msg)
			logger.on_message_received(vcu_msg)
			print(vcu_msg)
		except can.CanError:
			print('Message send error: {}'.format(str(can.CanError)))
		t_send = time.time()


# Terminate the bus
logfile.close()
os.system('sudo ifconfig can0 down')
