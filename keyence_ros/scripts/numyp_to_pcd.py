#!/usr/bin/env python2
import pcl
import pcl_helper
import pcl.pcl_visualization
import numpy as np
import pptk

import rospy
from sensor_msgs.msg import PointCloud2
import sensor_msgs.point_cloud2 as pc2

def read_pcd():
	print("read pcd start")
	profile_np = np.load('/home/benlee/Desktop/pptk_save.npy')
	print("load type :", type(profile_np))
	profile_pcd = pcl.load("/home/benlee/Desktop/pocket.pcd")

	return profile_np, profile_pcd

def visualize(pc_data):
	print("visualize start")
	centred = pc_data - np.mean(pc_data, 0)
	pc_centred = pcl.PointCloud()
	pc_centred.from_array(centred)

	visual = pcl.pcl_visualization.CloudViewing()
	visual.ShowMonochromeCloud(pc_centred,b'pc_data')
	v = True
	while v:
		v = not(visual.WasStopped())

def filter_remove():
	profile_np, profile_pcd = read_pcd() 

	num_point = len(profile_np) #number of points
	final = []
	for i in range(0, num_point):
		check =profile_np[i]
		changed_arr = np.where(check == np.inf, 0, check)

		final.append(list(changed_arr))
		profile_np[i] = changed_arr 
	#infinity value check
	final = np.array(final, dtype=np.float32) #list to numpy

	# point cloud numpy, pcd read
	ori_cloud = pcl.PointCloud(final) #numpy to pcl

	pcl.save(ori_cloud,"/home/benlee/Desktop/ori_cloud.pcd")
	print("before :", ori_cloud.size)
	print("passthrough filter start")
	passthrough = ori_cloud.make_passthrough_filter()
	passthrough.set_filter_field_name("z")
	passthrough.set_filter_limits(-35, -10)
	pass_filtered = passthrough.filter()
	pcl.save(pass_filtered,"/home/benlee/Desktop/pass_filtered.pcd")
	print("after :", pass_filtered.size)

	print("outlier save start")
	fil = profile_pcd.make_statistical_outlier_filter()
	print("filtered")
	fil.set_mean_k(50)
	fil.set_std_dev_mul_thresh(1.0)

	inliter_filtered = fil.filter() 
	pcl.save(inliter_filtered,"/home/benlee/Desktop/50_inliers.pcd")
	fil.set_negative(True)
	outlier_filtered = fil.filter()
	pcl.save(outlier_filtered,"/home/benlee/Desktop/50_ioutliers.pcd")

	#visualize(cloud_filtered)

	#statistical outlier filter result save

	v = pptk.viewer(final)
	v.set(point_size=0.01)
 
	pc = pcl.PointCloud(final) #numpy to pcl

	cloud_new = pcl_helper.pcl_to_ros(pc)

	#pointcloud XYZ to ROS PointCloud2 msg
	pub.publish(cloud_new)

	

if __name__=="__main__":
	rospy.init_node('cmm_test', anonymous=True)
	pub = rospy.Publisher("cmm_profile", PointCloud2, queue_size=1)
	filter_remove()
	rospy.spin()