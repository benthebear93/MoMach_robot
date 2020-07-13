#!/usr/bin/env python2
import numpy as np
import math
import matplotlib.pyplot as plt
import rospy
from std_msgs.msg import Float64, Int16
from nav_msgs.msg import Odometry
import time
mass = 40
feed = 0.0183
dt = 0.0001339
def motion_model(x):
    #x = 3x1 [dis, vel, f]
    dis = float(x[0][0]+x[1][0]*dt)
    vel = float(x[1][0])
    force = 0.5*(mass*(feed*feed - vel*vel))
    if vel > 0:
        force = force/(dt*vel)
    else:
        force = force/(-1*dt*vel)
    #print("force :    ", force)
    F = np.matrix([[dis], [vel], [force]])
    x = F  # F*x
    return x

def measurement(x):
    H = np.matrix([[1,0,0],[0,1, 0],[0, 0, 0]])
    z = H * x
    return z

def callback_kf(pose_data):
    global x_, p_
	#get_x = pose_data.pose.pose.position.x
    get_y = pose_data.pose.pose.position.y
	#vel_x = pose_data.twist.twist.linear.x
    vel_y = pose_data.twist.twist.linear.y	
    H = np.matrix([[1,0,0],[0,1, 0],[0, 0, 0]])
    x = np.matrix([[get_y],[vel_y], [0]])
    z = H * x

    #predict
    Q = np.matrix([[0.0003, 0, 0],[0, 0.0003, 0],[0, 0, 0.0003]])
    R = np.matrix([[4, 0, 0],[0, 4, 0], [0, 0, 4]])
    xpred = motion_model(x_)
    jF = jacobian_f(xpred)
    ppred = jF*p_*jF.T + Q

    #update
    zpred = measurement(xpred)
    y = z - zpred
    S = H*ppred*H.T + R
    K = ppred*H.T*np.linalg.inv(S)
    x_ = xpred + K*y
    p_ = (np.eye(len(x_))-K*H)*ppred

    #x_result, p_est = ekf_estimation(x_, p_, z)

    #print("update_data :", x_, x_.shape)
    print("force : ", x_[2][0])
    pub.publish(x_[0][0])
    pub2.publish(x_[1][0])
    pub3.publish(x_[2][0])

def jacobian_f(x):
    dis = float(x[0][0])
    vel = float(x[1][0])
    a = 0.5*(mass*(vel*vel - feed*feed))/(dt*dt*vel*vel)
    jF = np.matrix([[1, dt, 0],[0, 1, 0],[a, -mass*vel/(dt*vel), 0]])
    return jF

def ekf_estimation(x_, p_, z):
    # z from sensor
    # zpred from cal
    #predict
    Q = np.matrix([[0.0003, 0, 0],[0, 0.0003, 0],[0, 0, 0.0003]])
    R = np.matrix([[0.005, 0, 0],[0, 0.005, 0], [0, 0, 0.005]])
    xpred = motion_model(x_)
    jF = jacobian_f(xpred)
    ppred = jF*p_*jF.T + Q

    #update
    zpred = measurement(xpred)
    y = z - zpred
    S = H*ppred*H.T + R
    K = ppred*H.T*np.linalg.inv(S)
    x_ = xpred + K*y
    p_ = (np.eye(len(x_))-K*H)*ppred

    return x_, p_

def start():
	print("start get m")
	global end_time, start_time
	rospy.Subscriber("camera/odom/sample",Odometry,callback_kf)
	rospy.spin()

if __name__ =='__main__':
    rospy.init_node('kf', anonymous = True)
    x_ = np.matrix([[0.00001],[0.000001], [0.0]])
    p_ = np.eye(3)
    pub = rospy.Publisher('kf_x',Float64, queue_size =10)
    pub2 = rospy.Publisher('kf_vx',Float64, queue_size =10)
    pub3 = rospy.Publisher('force',Float64, queue_size =10)
    start()