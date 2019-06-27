#!/usr/bin/env python
from plutodrone.srv import *
import rospy
from std_msgs.msg import Float64
from std_msgs.msg import Int32

class request_data():
	"""docstring for request_data"""
	def __init__(self):
		rospy.init_node('drone_board_data')
		data = rospy.Service('PlutoService', PlutoPilot, self.access_data)
		
	def access_data(self, req):
		 print "accx = " + str(req.accX), "accy = " + str(req.accY), "accz = " + str(req.accZ)
		 print "gyrox = " + str(req.gyroX), "gyroy = " + str(req.gyroY), "gyroz = " + str(req.gyroZ)
		 print "magx = " + str(req.magX), "magy = " + str(req.magY), "magz = " + str(req.magZ)
		 print "roll = " + str(req.roll), "pitch = " + str(req.pitch), "yaw = " + str(req.yaw)
		 print "altitude = " +str(req.alt)
		 print "battery = " + str(req.battery), "Power Consumed = " + str(req.rssi)
		 rospy.sleep(.1)
		 return PlutoPilotResponse(rcAUX2 =1500)

test = request_data()
rospy.spin()
