import os
import time
import can
import binascii

# Init CAN
os.system('sudo ip link set can0 type can bitrate 250000')
os.system('sudo ifconfig can0 up')

# Bind socket
can0 = can.interface.Bus(channel='can0', bustype='socketcan_ctypes')

# Start the logger
logfile = open('logOBC.log', 'w')
logger = can.CanutilsLogWriter(logfile, channel='can0')

# Send BMS message
def send():
	frame = [0x0D, 0xAC, 0, 0x32, 0, 0, 0, 0]
	try:
		msg = can.Message(arbitration_id=0x1806E5F4, data=frame, extended_id=True)
		can0.send(msg)
		print(msg)
	except can.CanError:
		print('Message not sent')

# Listen for a while
print('Listen to the OBC...')
t0 = time.time()
while time.time() - t0 < 60:
	obc_msg = can0.recv(1)
	if obc_msg is not None:
		logger.on_message_received(obc_msg)
		print(obc_msg)
	else:
		print('No messages')
	send()


# Terminate the bus
logfile.close()
os.system('sudo ifconfig can0 down')
