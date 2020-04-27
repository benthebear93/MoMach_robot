#!/usr/bin/env python2
import rospy
import cv2
import numpy as np
import math
from std_msgs.msg import Float32
import time


class Spindle_tool:
    def __init__(self):
        self.curr_pos = -0.0
        self.Kp = 0.5
        self.Ki = 0#0.01
        self.Kd = 0.01
        self.p_val = 0
        self.i_val = 0
        self.vel = 0
        self.plot_pos =[]

    def vel_update(self, laser_output):
        error = laser_output - self.curr_pos
        print("error :",error)
        if(error < 0.001):
            end_time = time.time()
            arrival_time = end_time - start_time
            rospy.loginfo(arrival_time)
        self.p_val = error*self.Kp
        self.i_val = self.i_val + error*self.Ki*0.005
        self.vel = self.p_val + self.i_val
        print("vel : ", self.vel)
        return self.vel

    def pos_update(self):
        self.curr_pos = self.curr_pos + self.vel
        print("curr_pos", self.curr_pos)
        return self.curr_pos

def callback_PID(laser_output):
    print("curr_pos                                  ",tool.curr_pos)
    #global flag
    print("laser output : ", laser_output.data) 
    tool.vel_update(laser_output.data)
    pos = tool.pos_update()
    pos_plot.append(pos)
    print("tool.plot_pos :", pos_plot)
    print("pos :", pos, " (mm)")
    pub.publish(pos)


def get_laser_data():
    rospy.Subscriber("keyence_output_1",Float32,callback_PID)
    print("here?")
    #rospy.Subscriber("keyence_output_2",Float32,callback_PID)
    #rospy.Subscriber("keyence_output_3",Float32,callback_PID)
    rospy.spin()

if __name__=='__main__':
    rospy.init_node('pid_simul')
    start_time = time.time()
    pub = rospy.Publisher('tool_z_pos',Float32, queue_size =10)
    pos_plot =[]
    tool = Spindle_tool()
    get_laser_data()