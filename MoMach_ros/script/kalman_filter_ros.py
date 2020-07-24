import numpy as np
import math as math
import rospy
from std_msgs.msg import Float64, Int16
import time

class KF:
	def __init__(self, F = None, B = None, H = None, Q = None, R = None, P = None, x0 = None):
		self.n = F.shape[1]
		self.m = H.shape[1]

		self.F = F #number of state
		self.H = H
		self.B = 0 if B is None else B
		self.Q = np.eye(self.n) if Q is None else Q
		self.R = np.eye(self.n) if R is None else R
		self.P = np.eye(self.n) if P is None else P
		self.x = np.zeros((self.n, 1)) if x0 is None else x0

	def predict(self, u = 0):
		self.x = np.dot(self.F, self.x) + np.dot(self.B, u) #eq1 of prediction state
		self.P = np.dot(np.dot(self.F, self.P), self.F.T) + self.Q #eq2 of prediction of covariance
		return self.x

	def update(self, z):
		nu = y - np.dot(self.H, self.x) #residual
		S = self.R + np.dot(self.H, np.dot(self.P, self.H.T)) #innovaton covariance
		K = np.dot(np.dot(self.P, self.H.T), np.linalg.inv(S)) #kalman gain
		self.x = self.x + np.dot(K, y) #stat update
		I = np.eye(self.n)
		self.P = np.dot(np.dot(I - np.dot(K, self.H), self.P),(I - np.dot(K, self.H)).T) + np.dot(np.dot(K, self.R), K.T) #covariance update


def callback_kf(pose_data):
	dt = end_time - start_time
	F = np.array([[1, dt, 0, 0], [0, 1, 0, 0], [0, 0, 1, dt], [0, 0, 0, 1]])
	H = np.array([1, 0, 1, 0]).reshape(1, 4)
	Q = np.array([[0.05, 0.05, 0.0, 0.0], [0.05, 0.05, 0.0, 0.0], [0.0, 0.0, 0.0, 0.0], [0.0, 0.0, 0.0, 0.0]])
	R = np.array([0.5, 0.0, 0.5, 0.0]).reshape(1, 4)

	get_x = pose_dat.pose.pose.position.x
	get_y = pose_dat.pose.pose.position.y
	z = np.array([get_x, 0, get_y, 0]).reshape(1,4)
	kf = KF(F = F, H = H, Q = Q, R = R)
	predict_data = np.dot(H,kf.predict())
	kf.update(z)
	pub.publish(predict_data[0])
	pub2.publish(predict_data[2])


def get_measurement():
	rospy.Subscriber("odom",Float64,callback_xy)
	end_time = time.time()
	rospy.spin()


if __name__=='__main__':
	rospy.init_node('kf', anonymous=True)
	start_time = time.time()
	pub = rospy.Publisher('kf_x',Float64, queue_size =10)
	pub2 = rospy.Publisher('kf_y',Float64, queue_size =10)
	get_measurement()
