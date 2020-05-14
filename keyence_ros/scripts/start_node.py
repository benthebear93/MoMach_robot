#!/usr/bin/env python2
import rospy
import numpy as np
import math
from std_msgs.msg import Float64
import time


def start_or_stop():
    rate = rospy.Rate(1000)
    while not rospy.is_shutdown():
        flag = input("1 for start 0 for stop : ")
        pub.publish(flag)
        rate.sleep()

if __name__=='__main__':
    rospy.init_node('start_button', anonymous=True)
    pub = rospy.Publisher('button', Float64, queue_size =10)
    start_or_stop()