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
		#rospy.spin()
		self.yaw_pub = rospy.Publisher('data/yaw', Int32, queue_size=1)
		self.roll_pub = rospy.Publisher('data/roll', Int32, queue_size=1)
		self.pitch_pub = rospy.Publisher('data/pitch', Int32, queue_size=1)
		self.battery_pub = rospy.Publisher('data/battery', Float64, queue_size=1)
		self.rssi_pub = rospy.Publisher('data/rssi', Int32, queue_size=1)

	def access_data(self, req):
		 print "accx = " + str(req.accX), "accy = " + str(req.accY), "accz = " + str(req.accZ)
		 print "gyrox = " + str(req.gyroX), "gyroy = " + str(req.gyroY), "gyroz = " + str(req.gyroZ)
		 print "magx = " + str(req.magX), "magy = " + str(req.magY), "magz = " + str(req.magZ)
		 print "roll = " + str(req.roll), "pitch = " + str(req.pitch), "yaw = " + str(req.yaw)
		 print "altitude = " +str(req.alt)
		 print "battery = " + str(req.battery), "Power Consumed = " + str(req.rssi)
		 rospy.sleep(.1)
		 #Uncomment the stuff you want to be explicitly Published
		 #self.yaw_pub.publish(req.yaw)
		 #self.roll_pub.publish(req.roll)
		 #self.pitch_pub.publish(req.pitch)
		 #self.battery_pub.publish(req.battery)
		 #self.rssi_pub.publish(req.rssi)
		 return PlutoPilotResponse(rcAUX2 =1500)

test = request_data()
rospy.spin()
