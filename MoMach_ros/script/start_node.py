#!/usr/bin/env python2
import rospy
import numpy as np
import math
from std_msgs.msg import Float64
from std_msgs.msg import Int16
import time


def start_or_stop():
    rate = rospy.Rate(100) #10hz = 0.1s = 100ms
    while not rospy.is_shutdown():
        print("===================================")
        print("== 010 : start z axis correction ==")
        print("== 000 : stop  z axis correction ==")
        print("== 110 :     reset go forward    ==")
        print("== 111 :     reset go backward   ==")
        print("===================================")
        flag3, flag, flag2 = map(int, raw_input("put command : ").split())
        pub.publish(flag)
        rate.sleep()

if __name__=='__main__':
    rospy.init_node('start_button', anonymous=True)
    pub = rospy.Publisher('stop', Int16, queue_size =10)
    start_or_stop()