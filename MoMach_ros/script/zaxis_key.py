#!/usr/bin/env python
# modified teleop_twist_key package
# wrriten by Ben Lee/HaeGu Lee
# UNIST MHM.Lab

from __future__ import print_function

import roslib
import rospy
from std_msgs.msg import Int16
import sys, select, termios, tty

msg = """
Reading from the keyboard  and Publishing to Twist!
---------------------------
z axis keyboard interrupt:
        w        
        x    
   CTRL-C to quit
   s : stop 
"""

movebindings ={
		#z,th
	'w':(1,0), #up
	'x':(-1,0) #down
}

def getkey():
	tty.setraw(sys.stdin.fileno())
	select.select([sys.stdin], [], [], 0)
	key = sys.stdin.read(1)
	termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
	return key


if __name__=="__main__":
	settings = termios.tcgetattr(sys.stdin)

	pub = rospy.Publisher('zaxis_key_input', Int16, queue_size =10)
	rospy.init_node('zaxis_keyboard_interrupt')
	resolution = 1
	z = 0
	status = 0

	try :
		print(msg)
		while(1):
			key = getkey()
			if key in movebindings.keys():
				z = movebindings[key][0]
			else:
				z = 0
				if(key == '\x03'):  # \x03 = ^c
					break
					
			keyboard_input = z*resolution
			pub.publish(keyboard_input)
	except Exception as e:
		print(e)

	finally:
		keyboard_input = 0
		pub.publish(keyboard_input)
		termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)