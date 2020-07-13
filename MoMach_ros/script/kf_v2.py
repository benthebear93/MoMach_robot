#!/usr/bin/env python2
import numpy as np
import math as m
import rospy
from std_msgs.msg import Float64, Int16
from nav_msgs.msg import Odometry
import time
class KF:
	def __init__(self, F =None, B = None, H = None, Q = None, R = None, P = None, x0 = None):
		self.n = F.shape[1]
		self.m = H.shape[1]

		self.F = F # 4X4
		self.H = H # 2x4
		self.B = 0
		self.Q = np.eye(self.n) if Q is None else Q #4X4
		self.R = np.eye(self.n) if R is None else R #4X4
 		self.P = np.eye(self.n) if P is None else P #4X4
		self.x = np.zeros((self.n,1)) if x0 is None else x0 #4X1

	def predict(self, u = 0):
		self.x = np.dot(self.F, self.x) + np.dot(self.B, u) #4x1
		self.P = np.dot(np.dot(self.F, self.P), self.F.T) + self.Q #4x4
		return self.x

	def update(self, z):
		nu = z - np.dot(self.H, self.x)#residual  #4x1
		S = np.dot(self.H, np.dot(self.P,self.H.T)) + self.R #4x4
		K = np.dot(np.dot(self.P, self.H.T), np.linalg.inv(S)) #kalman gain #4x4
		self.x = self.x + np.dot(K, nu) #stat update #4x1
		#self.P = self.P - np.dot(K, np.dot(self.H, self.P))
		I = np.eye(self.n)
		self.P = np.dot(np.dot(I - np.dot(K, self.H), self.P),(I - np.dot(K, self.H)).T) + np.dot(np.dot(K, self.R), K.T)
		return self.x

def measurement_update():
	global get_x, get_y



def callback_kf(pose_data):
	get_x = pose_data.pose.pose.position.x
	get_y = pose_data.pose.pose.position.y
	vel_x = pose_data.twist.twist.linear.x
	vel_y = pose_data.twist.twist.linear.y	
	z = np.array([[get_x],[vel_x], [get_y], [vel_y]]) #measurement

	predict_data = np.dot(H,kf.predict())
	update_data = kf.update(z)
	print("update_data :", update_data, update_data.shape)
	pub.publish(update_data[0][0])
	pub3.publish(update_data[1][0])
	pub2.publish(update_data[2][0])
	pub4.publish(update_data[3][0])


def get_measurement():
	print("start get m")
	global end_time, start_time
	rospy.Subscriber("camera/odom/sample",Odometry,callback_kf)
	rospy.spin()


if __name__=='__main__':
	rospy.init_node('kf', anonymous=True)
	pub = rospy.Publisher('kf_x',Float64, queue_size =10)
	pub2 = rospy.Publisher('kf_y',Float64, queue_size =10)
	pub3 = rospy.Publisher('kf_vx',Float64, queue_size =10)
	pub4 = rospy.Publisher('kf_vy',Float64, queue_size =10)
	dt = 0.0001339
	#print("dt :",dt)
	F = np.array([[1, dt, 0, 0], [0, 1, 0, 0], [0, 0, 1, dt], [0, 0, 0, 1]])
	H = np.array([[1, 0, 0, 0],[0, 1, 0, 0],[0, 0, 1, 0],[0, 0, 0, 1]])#4x4
	Q = np.array([[0.00003, 0.0, 0.0, 0.0], [0.0, 0.00003, 0.0, 0.0], [0.0, 0.0, 0.00003, 0.0], [0.0, 0.0, 0.0, 0.00003]]) #4x4
	R = np.array([[1, 0.0,0.0,0.0], [0.0, 1,0.0,0.0],[0.0, 0.0, 1, 0.0],[0.0, 0.0, 0.0, 1]])
	x0 = np.array([[0], [0.0], [0], [0.0]]) #measurement
	kf = KF(F = F, H = H, Q = Q, R = R, x0 = x0)
	print("get m")
	get_measurement()
