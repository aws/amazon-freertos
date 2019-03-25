# -*- coding:utf-8 -*-
#
# W600 flash download script
# Copyright (c) 2018 Winner Micro Electronic Design Co., Ltd.
# All rights reserved.
# 
# Python 3.0    (Special 3.4)
#
# pip install pyserial
# pip install pyprind
# pip install xmodem
#   pip install enum34    # for 2.7
# 
## pip install PyInstaller
## pyinstaller -F download.py -i Downloads_folder_512px.ico
#

import serial
import struct
import platform
#import pyprind
import os
import sys, getopt
import time
from xmodem import XMODEM1k
import threading
from time import ctime,sleep
from enum import IntEnum

COM_NAME="COM1"
IMG_FILE="WM_W600_GZ.img"
reset_success=0
BAUD_SPEED=115200
#serial_mutex = threading.Lock()

Major_version=0
Minor_version=2
Reversion_num=2

class m_err(IntEnum):
	success = 0,
	err_serial_obj_init = -1,
	err_serial_open = -2,
	err_serial_read = -3

class WMDownload(object):
	
	if platform.system() == 'Windows':
		DEFAULT_PORT = "COM1"
	else:
		DEFAULT_PORT = "/dev/ttyUSB0"
	DEFAULT_BAUD = 115200
	DEFAULT_TIMEOUT = 0.3
	DEFAULT_IMAGE = "../Bin/WM_W600_GZ.img"
	
	def __init__(self, port=DEFAULT_PORT, baud=DEFAULT_BAUD, timeout=DEFAULT_TIMEOUT, image=DEFAULT_IMAGE):
		try:
			self.image = image
			self.ser = serial.Serial(port, baud, timeout=timeout)
			statinfo_bin = os.stat(image)
			#self.bar_user = pyprind.ProgBar(statinfo_bin.st_size/1024+2, stream=1)
		except serial.serialutil.SerialException:
			print('Serial %s is used by other or INIT error' % port)
			sys.exit(m_err['err_serial_obj_init'])
	
	def image_path(self):
		return self.image
	
	def set_port_baudrate(self, baud):
		self.ser.baudrate = baud
	
	def set_timeout(self, timeout):
		self.ser.timeout  = timeout
	
	def getc(self, size, timeout=1):
		try:
			ret=self.ser.read(size)
		except serial.serialutil.SerialException:
			print("OFFline Serial %s" % self.ser.port)
			sys.exit(m_err["err_serial_read"])
		return ret
	
	def putc(self, data, timeout=1):
		self.ser.timeout = timeout
		try:
			self.ser.write(data)
		except serial.serialutil.SerialTimeoutException:
			# do nothing
			# try it again
			return 
		
	def rst_putc(self, data, timeout = 1):
		self.ser.timeout = timeout
		try:
			self.ser.write(data)
		except serial.serialutil.SerialException as err:
			print('[ ', sys._getframe().f_lineno, ' ]', err)
			# do nothing
			# maybe serial is closed
			return
	
	def rst_flushinput(self):
		try:
			self.ser.flushInput()
		except serial.serialutil.SerialException:
			# do nothint
			# maybe serial is closed
			return
	
	def putc_bar(self, data, timeout=1):
		self.ser.timeout = timeout
		#self.bar_user.update()
		return self.ser.write(data)
	
	def open(self):
		try:
			self.ser.open()
		except serial.serialutil.SerialException:
			print("Open Serial %s error" % self.ser.port)
			sys.exit(m_err["err_serial_open"])
	
	def close(self):
		self.ser.flush()
		self.ser.flushInput()
		self.ser.close()
	
	def info(self):
		return (self.ser.port , self.ser.baudrate)

def print_progress_bar(current, total, width=30):
	count=width/total*current;
	left=width-count;
	level=current*100/total
	sys.stdout.write("%3d"%(level))
	#sys.stdout.flush();
	sys.stdout.write('% [');
	#sys.stdout.flush();
	while (count > 0):
		sys.stdout.write('#');
		#sys.stdout.flush();
		count = count - 1;
	while (left > 0):
		sys.stdout.write(' ');
		#sys.stdout.flush();
		left = left - 1;
	sys.stdout.write('] 100%\r');
	sys.stdout.flush();
	
def print_point(count):
	if ((count - 1) % 20 == 0 and count > 1):
		print();
	elif ((count - 1) % 10 == 0):
		print("   ", end='');
	elif ((count - 1) % 5 == 0):
		print(' ', end='');
	print('. ', end='');
	sys.stdout.flush();
	
def wait_for_secs(seconds, print_flag=1):
	if (print_flag == 1):
		print("Please wait for about %s seconds before module restart ..." % seconds)
	#print("Elapse ", end='')
	count = 1;
	while (count <= seconds):
		#print_progress_bar(count, seconds);
		print_point(count);
		time.sleep(1);
		count = count + 1;
	print();
	#for pro in pyprind.prog_bar(range(50)):
	#	time.sleep(0.2)
	#print("FINISH ")
	
def print_version():
	print("download %d.%d.%d" %(Major_version, Minor_version, Reversion_num));
	
def usage():
	print_version();
	print('USAGE:')
	print('win:python download.py [-c|--console= COM] [-f|--image-file= image]')
	print('Linux:python3 download.py [-c|--console= COM] [-f|--image-file= image]')
	print()
	print('\t-c|--console=    COM         the console used by download function.')
	print('\t                             default：\"COM1\" for windows, \"ttyUSB0\" for linux.')
	print('\t-b|--baud-speed= BAUD_SPEED  the baud speed selected by user.')
	print('\t                             default: 115200')
	print('\t-f|--image-file= IMAGE       the image file used by download function transmitted to the DUT.')
	print('\t                             default: \"../Bin/WM_W600_GZ.img\"')
	print('\t-h|--help                    get this help information')
	print('\t-V|--version                 get version number')
	print()
	
def get_args():
	opts, args = getopt.getopt(sys.argv[1:], "c:b:f:hV", ["console=", "baud-speed=", "image-file=", "help", "version"])
	for op, value in opts:
		#print(op)
		if "-c" == op or "--console" == op:
			global COM_NAME
			COM_NAME = value
		elif "-b" == op or "--baud-speed" == op:
			global BAUD_SPEED
			BAUD_SPEED = value
		elif "-f" == op or "--image-file" == op:
			global IMG_FILE
			IMG_FILE = value
		elif "-h" == op or "--help" == op:
			usage()
			sys.exit()
		elif "-V" == op or "--version" == op:
			print_version();
			sys.exit();
			
def soft_restart_target(serial):
	serial.rst_putc(struct.pack('<B', 97))		# 'a'
	serial.rst_putc(struct.pack('<B', 116))		# 't'
	serial.rst_putc(struct.pack('<B', 43))		# '+'
	serial.rst_putc(struct.pack('<B', 122))		# 'z'
	serial.rst_putc(struct.pack('<B', 10))		# '\n'
	serial.rst_flushinput()			# clear input buffer
	
def send_esc_to_target(serial):
	count = 0;
	while ( count < 1500 ):
		serial.putc(struct.pack('<B', 27))	# ESC
		serial.rst_flushinput()			# clear input buffer
		count = count + 1
		
def restart_w600(serial):
	global reset_success
	idx=0
	#print(sys._getframe().f_lineno)
	time.sleep(2)
	#print(sys._getframe().f_lineno)
	global serial_mutex
	while True:
		soft_restart_target(serial);
		send_esc_to_target(serial);
		
		c = serial.getc(1, 0.01)
		#print(c)
		if c == b'C':
			print("The target is waiting for the firmware file ...");
			sys.stdout.flush();
			break
			
		if idx % 4 == 1:
			print('\r-', end='')
			sys.stdout.flush();
		elif idx % 4 == 2:
			print('\r\\', end='')
			sys.stdout.flush();
		elif idx % 4 == 3:
			print('\r|', end='')
			sys.stdout.flush();
			idx=0
		elif idx % 4 == 0:
			print('\r/', end='')
			sys.stdout.flush();
		sys.stdout.flush()
		idx = idx + 1	
	
def upgrade_firmware(serial):
	print("Start upgrade %s "  % serial.image_path());
	sys.stdout.flush();
	try:
		stream = open(serial.image_path(), 'rb+')
	except IOError:
		print("Can't open %s file." % serial.image_path());
		sys.stdout.flush();
		serial.close()
		raise
	else:
		serial.set_timeout(1)
		time.sleep(0.2)
		modem = XMODEM1k(serial.getc, serial.putc_bar)
		print("Please wait for upgrade ...");
		sys.stdout.flush();
		# try: modem.send() ...
		result = modem.send(stream)
		#stream.tell()
		#wait_for_secs(15);
		#print('')
		if result:
			print("Upgrade %s image success!" % serial.image_path());
			sys.stdout.flush();
			print("Please wait for about 10 seconds before uncompress & restart ...")
			wait_for_secs(10, print_flag=0)
		else:
			print("Upgrade %s image fail!" % serial.image_path());
			sys.stdout.flush();
			sys.exit(1);
		serial.close()

def try_change_baud_speed(serial):
	try_count=0
	
	serial.close()
	time.sleep(0.2)
	serial.set_port_baudrate(2000000)
	serial.open()
	time.sleep(0.2)
		
	print('Try to use 2Mbps to burn the firmware ...')
	sys.stdout.flush();
	
	# check baudrate
	while ( try_count < 1500 ):
		c = serial.getc(1)
		if c == b'C':
			print('Serial into high speed mode');
			sys.stdout.flush();
			break
		else:
			serial.close()
			serial.set_port_baudrate(115200)
			serial.open()
			time.sleep(0.2)
			serial.putc(bytes.fromhex('210a00ef2a3100000080841e00'))
			serial.close()
			serial.set_port_baudrate(2000000)
			serial.open()
			time.sleep(0.2)
			try_count = try_count + 1
	
	if ( try_count >= 1500 ):
		global BAUD_SPEED
		time.sleep(0.5)
		print('Use baud speed %s on the serial port.' % BAUD_SPEED)
		sys.stdout.flush();
		serial.close()
		serial.set_port_baudrate(BAUD_SPEED)
		serial.open()
		time.sleep(0.5)
	
def main(argv):
	get_args()
	
	download = WMDownload(port=COM_NAME, baud=BAUD_SPEED, image=IMG_FILE)
	
	#print('')
	sys.stdout.flush();
	print("Serial open success！com: %s, baudrate: %s." % download.info())
	sys.stdout.flush();
	print('Waiting for restarting device ...')
	sys.stdout.flush();
	
	download.set_timeout(0.1)
	
	restart_w600(download);
	try_change_baud_speed(download)
	upgrade_firmware(download)
	
if __name__ == '__main__':
    main(sys.argv)
