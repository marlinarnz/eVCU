import os
import time
import can
import binascii

# Init CAN
os.system('sudo ip link set can0 type can bitrate 500000')
os.system('sudo ifconfig can0 up')

# Bind socket
can0 = can.interface.Bus(channel='can0', bustype='socketcan_ctypes')

# Start the logger
logfile = open('logOBC.log', 'w')
logger = can.CanutilsLogWriter(logfile, channel='can0')

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

'''
# Create message BMS1
frame = [0, 0, 0, 0, 0, 0, 0, 0]
def get_msg(frame):
	return can.Message(arbitration_id=0x520, data=frame, extended_id=False)
frame[4] = 1 # set charger off

try:
	print('Send message...')
	msg = get_msg(frame)
	can0.send(msg)
	print(msg)
	print(can0.channel_info)
except can.CanError:
	print('Message not sent')
'''

# Terminate the bus
logfile.close()
os.system('sudo ifconfig can0 down')
