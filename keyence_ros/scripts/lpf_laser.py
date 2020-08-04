#!/usr/bin/env python2
import math as m
import rospy
from std_msgs.msg import Float32, Float64, Int16
import numpy as np
import time

#end_time = 0 
#start_time = 0

class Laser_data:
    def __init__(self):
        self.pre_val = 0
        self.curr_val = 0

    def LowPassFilter(self, tau):
        self.curr_val = (tau*self.pre_val + self.curr_val*ts)/(tau + ts)

def callback_kf(pose_data):
    #global end_time, start_time
    #start_time = time.time()
    get_z = pose_data.data
    laser.curr_val = get_z
    laser.LowPassFilter(0.01)
    pub.publish(laser.curr_val)
    laser.pre_val = laser.curr_val
    #end_time = time.time()
    #print("ts : ", end_time - start_time)

def get_measurement():
    print("start get m")
    rospy.Subscriber("mid_laser",Float32,callback_kf)
    rospy.spin()

if __name__=='__main__':
    rospy.init_node('lpf_laser', anonymous=True)
    pub = rospy.Publisher('lpf_z',Float32, queue_size =10)
    ts = 0.0001
    print("dt :",ts)
    laser = Laser_data()
    get_measurement()
