#!/usr/bin/env python
from plutodrone.srv import *
from plutodrone.msg import *
from std_msgs.msg import Int16
import rospy

class send_data():
	"""docstring for request_data"""
	def __init__(self):
		rospy.init_node('drone_server')
		self.command_pub = rospy.Publisher('/drone_command', PlutoMsg, queue_size=1)


		self.key_value =0
		self.cmd = PlutoMsg()
		self.cmd.rcRoll =1500
		self.cmd.rcPitch = 1500
		self.cmd.rcYaw =1500
		self.cmd.rcThrottle =1500
		self.cmd.rcAUX1 =1500
		self.cmd.rcAUX2 =1500
		self.cmd.rcAUX3 =1500
		self.cmd.rcAUX4 =1000

		rospy.Subscriber('/input_key', Int16, self.indentify_key )
		
	def arm(self):
		self.cmd.rcRoll=1500
		self.cmd.rcYaw=1500
		self.cmd.rcPitch =1500
		self.cmd.rcThrottle =1000
		self.cmd.rcAUX4 =1500
		self.command_pub.publish(self.cmd)
		rospy.sleep(1)
		
	def disarm(self):
		self.cmd.rcThrottle =1300
		self.cmd.rcAUX4 = 1200
		self.command_pub.publish(self.cmd)
		rospy.sleep(1)
	
	def indentify_key(self, msg):
		self.key_value = msg.data

		print "msg",self.key_value
		if self.key_value == 0:         
			self.disarm()
		if self.key_value == 70:
			self.disarm()
			self.arm()
		if self.key_value == 10:
			self.forward()
		if self.key_value == 20:
			self.reset()
		if self.key_value == 30:
			self.left()
		if self.key_value == 40:
			self.right()
		if self.key_value == 80:
			self.reset()
		if self.key_value == 50:
			self.increase_height()
		if self.key_value == 60:
			self.decrease_height()
		if self.key_value == 110:
			self.backward()
		self.command_pub.publish(self.cmd)

	def forward(self):
		self.cmd.rcPitch =1600
		self.command_pub.publish(self.cmd)
	def backward(self):
		self.cmd.rcPitch =1400
		self.command_pub.publish(self.cmd)
	def left(self):
		self.cmd.rcRoll =1600
		self.command_pub.publish(self.cmd)	
	def right(self):
		self.cmd.rcRoll =1400
		self.command_pub.publish(self.cmd)
	def reset(self):
		self.cmd.rcRoll =1500
		self.cmd.rcThrottle =1500
		self.cmd.rcPitch =1500
		self.cmd.rcYaw = 1500
		self.command_pub.publish(self.cmd)
	def increase_height(self):
		self.cmd.rcThrottle = 1600
		self.command_pub.publish(self.cmd)
	def decrease_height(self):
		self.cmd.rcThrottle =1400
		self.command_pub.publish(self.cmd)	


if __name__ == '__main__':
	test = send_data()
	while not rospy.is_shutdown():
		# test.control_drone()
		rospy.spin()
		sys.exit(1)


