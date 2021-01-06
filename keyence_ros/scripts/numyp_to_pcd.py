#!/usr/bin/env python2

import pcl
import pcl_helper
import numpy as np
import pptk

import rospy
from sensor_msgs.msg import PointCloud2
import sensor_msgs.point_cloud2 as pc2

def read_pcd():
	print("read pcd start")
	profile_load = np.load('/home/benlee/Desktop/pptk_save.npy')
	profile_pcd = pcl.load("/home/benlee/Desktop/pocket.pcd")

	fil = profile_pcd.make_statistical_outlier_filter()
	fil.set_mean_k(10)
	fil.set_std_dev_mul_thresh(1.0)

	pcl.save(fil.filter(),
				"/home/benlee/Desktop/inliers.pcd")

	fil.set_negative(True)
	pcl.save(fil.filter(),
				"/home/benlee/Desktop/outliers.pcd")

	print(profile_load)
	final = []
	for i in range(0, (929600)):
		check =profile_load[i]
		#print(check)
		changed_arr = np.where(check == np.inf, 0, check)
		final.append(list(changed_arr))
		profile_load[i] = changed_arr
	final = np.array(final, dtype=np.float32)

	pc = pcl.PointCloud(final)
	cloud_new = pcl_helper.pcl_to_ros(pc)
	#cloud_new = pcl_helper.XYZRGB_to_XYZ(cloud_new)
	pub.publish(cloud_new)

	#print("pcl.pointcloud form array", pc, type(pc))
	#print(pc)

if __name__=="__main__":
	rospy.init_node('cmm_test', anonymous=True)
	pub = rospy.Publisher("cmm_profile", PointCloud2, queue_size=1)
	read_pcd()
	rospy.spin()
#visualize 
#v = pptk.viewer(final)
#v.set(point_size = 0.00001)