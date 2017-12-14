#!/usr/bin/env python
import rospy
import roslib
from std_msgs.msg import Char,Int16

import sys, select, termios, tty

msg = """
Control Your Drone!
---------------------------
Moving around:
   u    i    o
   j    k    l
   n    m    ,


a : Arm drone
d : Dis-arm drone
r : stop smoothly
w : increase height
s : increase height

CTRL-C to quit
"""

"""
Function Name: getKey
Input: None
Output: keyboard charecter pressed
Logic: Determine the keyboard key pressed
Example call: getkey()
"""
def getKey():
    tty.setraw(sys.stdin.fileno())
    rlist, _, _ = select.select([sys.stdin], [], [], 0.1)
    if rlist:
        key = sys.stdin.read(1)
    else:
        key = ''

    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key

if __name__=="__main__":
    settings = termios.tcgetattr(sys.stdin)
    rospy.init_node('simple_drone_teleop_key')
    pub = rospy.Publisher('/input_key',Int16, queue_size=5) #publish the key pressed
    rate=rospy.Rate(8)
    msg_pub=0
    keyboard_control={  #dictionary containing the key pressed abd value associated with it
                      'i': 10,
                      'k': 20,
                      'j': 30,
                      'l': 40,
                      'd':0, 
                      'w':50,
                      's':60,
                      'a':70,
                      'r':80,
                      't':90,
                      'p':100,
                      'm':110,
                      'n':120}

    control_to_change_value=('u','o',',','z','c') #tuple containing the key that change the value

    try:
        pass
        # print value()
        while not rospy.is_shutdown():
            key = getKey()
            if key in keyboard_control.keys():
                msg_pub=keyboard_control[key]
                if key in control_to_change_value:
                    print "control_value"
            #else:
                #msg_pub=-1
            #rospy.loginfo(key);
            pub.publish(msg_pub)
            rate.sleep()
    except Exception as e:
        print e
    finally:
        print key
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
