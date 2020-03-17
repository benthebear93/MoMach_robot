import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as numpy
import rospy
import math

class grid4path:
	def __init__(self):
		ax = plt.axes(xlim = (-3000, 3000), ylim =(-3000, 3000))
		plt.grid()

		plt.xlabel('x_axis')
		plt.ylabel('y_axis')
		plt.title('Work Space')
		self.xlog = []
		self.ylog = []

	def draw(self):
		plt.clf()
		self.x_current = 0
		self.y_current = 0

		self.xlog(x_current)
		self.ylog(y_current)

		plt.plot(x, y)
		plt.grid()
		plt.xlabel('X_axis')
		plt.title('Work_Space')
		plt.show()

if __name__ == '__main__':
	rospy.init_node('path_drawing')
	test = grid4path()	
