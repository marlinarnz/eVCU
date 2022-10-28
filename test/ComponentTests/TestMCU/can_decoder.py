import time
import xlrd


class Decoder():
	
	def __init__(self, filter_on=False, filter_cutoff=8):
		# Load the Excel file with CAN message translation
		self.book = xlrd.open_workbook(
			'APEV528 CAN MsgList_V3_Excerpt.xlsx',
			on_demand=True)
		self.sheets = {str(x): None
			for x in self.book.sheet_names()}
		self.filter = filter_on
		self.filter_cut = filter_cutoff
		self.messages = []
	
	def __out(self, msg):
		print(msg)

	def __get_sheet_name(self, ident, prefix):
		# Return the sheet name for given ID
		name = [idx for idx, _ in self.sheets.items()
			if idx.startswith(prefix + str(ident))]
		if name == []:
			raise ValueError('No sheet found to translate ID ' + str(ident))
		return name[0]

	def __get_sheet(self, name):
		return self.book.sheet_by_name(name)

	def __get_sheet_dict(self, ident, prefix='$'):
		# Return a dict with message parsing information
		# Create the dict if not yet happened
		name = self.__get_sheet_name(ident, prefix)
		if self.sheets[name] is None:
			sheet = self.__get_sheet(name)
			conv_col = sheet.col_values(1, start_rowx=5, end_rowx=None)
			conv_col = [tuple([int(i) for i in s[1:-1].strip().split(',')])
				for s in conv_col]
			name_col = sheet.col_values(0, start_rowx=5, end_rowx=None)
			self.sheets[name] = {c:n for c,n in zip(conv_col, name_col)}
		return self.sheets[name]

	def get_signal_name(self, ident, lsb, len):
		# Return human readable name of a signal
		sheet_dict = self.__get_sheet_dict(ident)
		return sheet_dict[(lsb, len)]

	def get_msg_struct_list(self, ident):
		# Return a list with the message structure
		sheet_dict = self.__get_sheet_dict(ident)
		struct_list = [k for k, v in sheet_dict.items()]
		# Order by lsb
		def lsb(e):
			return e[0]
		struct_list.sort(key=lsb)
		return struct_list

	def decode(self, ident, bytes, sep='; '):
		# Return a string with decoded signals
		try:
			sheet_dict = self.__get_sheet_dict(ident)
			ret_string = '0x'+str(ident)+sep
			for signal in self.get_msg_struct_list(ident):
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
				ret_string = ret_string + sheet_dict[signal] + ': ' + str(val) + sep
			if ret_string[-2:]==sep:
                            ret_string = ret_string[:-2]
			return ret_string
		except ValueError as e:
			raise e

	def set_filter(self, on):
		# Filter message output on a running bus
		if not isinstance(on, bool):
			raise TypeError
		self.filter = on

	def set_filter_cutoff(self, cut):
		# Filter sensitivity
		if not isinstance(cut, int):
			raise TypeError
		self.filter_cut = cut

	def apply_filter(self, string):
		# Filter messages that did not change
		if self.filter:
			if len(self.messages) > self.filter_cut:
				self.messages = self.messages[-self.filter_cut:]
			if string[:-12] not in self.messages: # Without counter and checksum
				self.messages.append(string[:-12])
				return string
		else:
			return string

	def get_timestamp(self, string, keep_time, type='log'):
		# Retreive a timestamp from time or string
		if keep_time is None:
			return ''
		elif keep_time is True: # Keep the original string's timestamp
			if type=='log':
				return string.split()[0] + ' '
			if type=='message':
				return string.split()[1] + ' '
		else:
			return str(time.time())[7:-1] + ' '

	def decode_log_line(self, string, keep_time=True, sep='; '):
		# Decode a line from a log file of the can-utils library
		msg = string.split()[-1].split('#')
		msg = self.decode(msg[0],
			[int('0x'+msg[1][i:i+2], 16) for i in range(0, len(msg[1]), 2)],
			sep)
		return self.get_timestamp(string, keep_time, type='log') + \
			self.apply_filter(msg)

	def decode_message(self, string, keep_time=True, sep='; '):
		# Decode a message given as a String
		bytes = string.split('DLC:')[-1].split()
		id = string.split('ID:')[-1].split()[0]
		msg = self.decode(hex(int(id, 16))[2:],
                                  [int(bytes[i], 16) for i in range(1, int(bytes[0])+1)], sep)
		return self.get_timestamp(string, keep_time, type='message') + \
			self.apply_filter(msg)


def test():
	# Test the decoder
	d = Decoder()
	print(d.decode(105, [0x00,0x00,0x00,0x00,0x00,0x40,0x4A,0x75]))
	print(d.decode_log_line('(1609668659.884186) can0 105#0000000000404A75'))
	print(d.decode(106, [0x03,0x36,0x00,0x00,0x00,0x00,0x02,0xC4]))

def test_file():
	# Test decoding a file with output on the terminal
	d = Decoder(True)
	with open('logMCU.log') as f:
		for l in f:
			time.sleep(0.01) # Emulate MCU behaviour
			msg = d.decode_log_line(l[:-2], False)
			if msg:
				print(msg)#, end='\r')

def decode_log():
	# Save a decoded log file
	d = Decoder(False)
	with open('logMCU.log', 'r') as f:
		with open('logMCU_decoded.log', 'w') as f_d:
			for l in f:
				try:
					f_d.write(d.decode_log_line(l) + '\n')
				except:
					pass

if __name__ == "__main__":
    decode_log()
