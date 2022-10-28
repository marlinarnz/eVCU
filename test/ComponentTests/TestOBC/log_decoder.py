import xlrd


def decode_str(matrix, msg_str):
	# Return a string with message content based on matrix map
	map = get_map(matrix)
	bytes = get_msg_bytes(msg_str)
	ret_str = msg_str + ' --> '
	for name, signal in map.items():
		val = None
		# Assume the signal to be full bytes only or within one byte
		if signal[1] > 8:
			b_list = [bytes[int(signal[0]/8)-1 + i] for i in range(int(signal[1]/8))]
			val = int(b_list[0])
			for b in b_list[1:]:
				val = (val << 8) | int(b)
		elif signal[1] == 8:
			val = int(bytes[int(signal[0]/8)])
		else:
			val = (bytes[int(signal[0]/8)] & (0x00FF >> (8-(signal[0]%8+signal[1])))) >> (signal[0]%8)
		ret_str = ret_str + name + ': ' + str(round(val*signal[2]+signal[3], 1)) + '; '
	return ret_str[:-2]

def get_map(ident):
	# Return dict with signal name and tuple of lsb, len for given ID
	sheet = book.sheet_by_name(ident)
	names = sheet.col_values(0, start_rowx=1, end_rowx=None)
	lsbs = sheet.col_values(3, start_rowx=1, end_rowx=None)
	lens = sheet.col_values(4, start_rowx=1, end_rowx=None)
	convs = sheet.col_values(6, start_rowx=1, end_rowx=None)
	offsets = sheet.col_values(7, start_rowx=1, end_rowx=None)
	signal_dict = {names[i]: (int(lsbs[i]), int(lens[i]), float(convs[i]), int(offsets[i]))
		for i in range(len(names))}
	# Sort by lsb
	def lsb(item):
		return item[1][0]
	return {k:v for k,v in sorted(signal_dict.items(), key=lsb)}

def get_msg_bytes(msg_str):
	# Return list of bytes from message string
	return [int('0x'+msg_str[i:i+2], 16) for i in range(0, len(msg_str), 2)]

# Save messages from all logs into one dict
log_messages = {
	'logOBC.txt': {},
	'logOBC_no_BMS.log': {},
	'logOBC_no_battery_no_BMS.log': {}
}

for log, msg_dict in log_messages.items():
	with open(log) as file:
		msgs = []
		for line in file:
			msg = line.split()[-1]
			# Save every message once
			if msg not in msgs:
				msgs.append(msg)
				msg = msg.split('#')
				# Update the dictionary
				if msg[0] not in msg_dict.keys():
					msg_dict.update({msg[0]: [msg[1]]})
				else:
					msg_dict[msg[0]].append(msg[1])

# Print out general information
for log, msgs in log_messages.items():
	print(log + ' IDs: ' + str(msgs.keys()))

# Decode messages
book = xlrd.open_workbook('OBC_CAN_Matrix.xlsx', on_demand=False)
# Write them into a file
with open('logs_decoded.txt', 'w') as out:
	out.writelines(['Matching log messages with possible decoding matrices',
		'\n\n-----------', '\n'])
	for log, msg_dict in log_messages.items():
		out.write(log + ':\n\n')
		# Decode BMS message
		if log == 'logOBC.txt':
			out.write('  BMS to OBC message(s):\n')
			for msg in msg_dict['1806E5F4']:
				out.write('    ' + decode_str('BMS_OBC', msg) + '\n\n')
			msg_dict.pop('1806E5F4')
		# Decode the OBC messages with different maps
		for map in ['OBC_BMS_'+str(i) for i in [1,2,3]] + ['TC']:
			out.write('  ' + map + ':\n')
			for name, msgs in msg_dict.items():
				out.write('    ' + name + ':\n')
				for msg in msgs:
					out.write('    ' + decode_str(map, msg) + '\n')
			out.write('\n')
		out.write('\n-----------\n\n')
