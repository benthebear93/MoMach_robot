#!/usr/bin/env python2
""" 
profile_check node is for calibrating Laser sensor(LJ-V7000, Keyence)

Author : Haegu Lee

"""
import math as m
import rospy
from std_msgs.msg import Float32, Float64, Int32
import numpy as np
import time
from MoMach_ros.msg import pos_stream
end_time=0
start_time=0

class Laser_data:
    def __init__(self):
        self.pre_val = 0
        self.curr_val = 0
        self.gradient = 0
        self.count = 0

    def Gradient_cal(self, dt):
        #print("pre value :", self.pre_val, "current value :", self.curr_val, "time :", dt)
        self.gradient = (self.pre_val + self.curr_val)/(dt)
        #print("gradient value :", self.gradient)


def callback(pose_data):
    global end_time, start_time,count
    laser.curr_val = pose_data.data
    laser.count = laser.count+1
    #print("laser count ", laser.count)
    one_time_rotation = 314.159/0.833
    w = -2*m.pi/one_time_rotation
    error_com = -0.25*m.sin(w*time.time())-0.25*m.sin(9*w*time.time())
    end_time=time.time()
    pub.publish(error_com)
    '''if(laser.count%100==0):
        start_time = time.time()
        dt = end_time-start_time
        laser.Gradient_cal(dt)
        pub.publish(laser.gradient)
        laser.pre_val = laser.curr_val
        laser.count = 0'''

def sub_pos_callback(y_data):
    one_time_rotation = 314.159/0.833
    w = 2*m.pi/one_time_rotation
    print("pos_stream", y_data.data)
    t = y_data.data/0.833
    print("t", t)
    error_com = -0.25*m.sin(w*t)-0.25*m.sin(9*w*t)/7
    pub2.publish(error_com)

def get_measurement():
    rospy.Subscriber("mid_laser",Float32,callback)
    rospy.Subscriber("posy", Float32, sub_pos_callback)
    end_time=time.time()
    rospy.spin()

if __name__=='__main__':
    rospy.init_node('profile_check', anonymous=True)
    pub = rospy.Publisher('error_com1',Float32, queue_size =10)
    pub2 = rospy.Publisher('error_com2',Float32, queue_size =10)
    laser = Laser_data()
    print("gradient :", laser.gradient)
    get_measurement()
    