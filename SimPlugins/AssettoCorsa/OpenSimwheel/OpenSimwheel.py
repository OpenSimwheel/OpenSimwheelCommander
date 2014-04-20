
import string
import math
import ac
import acsys
#mport os.path
#import os
import struct                          # for converting from bytes to useful data
import mmap                            # for writing shared memory

filename = "Local\\AssettoCorsaOpenSimwheel"
#fileHandle = open(filename, 'w')
memFile = None
tick = 0

def acMain(ac_version):
	ac.log("AC Main / OpenSimwheel")
	global memFile
	global tick
	tick = tick + 1
	memFile = mmap.mmap(-1, 50, filename, access=mmap.ACCESS_WRITE)
	buf = struct.pack("?if", True, tick,0)
	memFile.seek(0)
	memFile.write(buf)
	
	return "OpenSimwheel v0.1"

def acShutdown():
	global memFile
	global tick
	tick = tick + 1
	buf = struct.pack("?if", False, tick, 0);
	memFile.seek(0)
	memFile.write(buf)
	
	memFile.close()
	ac.log("shutdown")
  
def acUpdate(deltaT):
	global tick
	global memFile
	tick = tick + 1
	forceValue=ac.getCarState(0,acsys.CS.LastFF)
	#steeringValue=ac.getCarState(0,acsys.CS.Steer)
	#buf = ctypes.create_string_buffer(10)
	buf = struct.pack("?if", True, tick,forceValue)
	memFile.seek(0)
	memFile.write(buf)
	
	#ac.log(str(forceValue))
	
