#!/usr/bin/env python2
import pcl
import pcl_helper
import pcl.pcl_visualization
import numpy as np
import pptk

import rospy
from sensor_msgs.msg import PointCloud2
import sensor_msgs.point_cloud2 as pc2

def do_smoothihg(pc_data):
	tree = pc_data.make_kdtree()
	mls = pc_data.make_moving_least_squares()
	mls.set_Compute_Normals (True)
	mls.set_polynomial_fit (True)
	mls.set_Search_Method (tree)
	mls.set_search_radius (0.05) # Use all neighbors in a radius of 3cm.
	mls_points = mls.process ()

	return mls_points

def do_voxel_grid_downsampling(pc_data, leaf_size):
	print("voxel start")
	vox = pc_data.make_voxel_grid_filter()
	vox.set_leaf_size(leaf_size, leaf_size, leaf_size)

	return vox.filter()

def do_pass_through_filtering(pc_data):
	print("passthroguh start")
	passthrough = pc_data.make_passthrough_filter()
	passthrough.set_filter_field_name("z")
	passthrough.set_filter_limits(-35, -10)

	return passthrough.filter()

def do_statistical_outlier_filter(pc_data, mean, std):
	print("do statistical outlier filter")
	fil = pc_data.make_statistical_outlier_filter()
	#gaussian setting
	fil.set_mean_k(mean)
	fil.set_std_dev_mul_thresh(std)

	inliter_filtered = fil.filter() 
	#pcl.save(inliter_filtered,"/home/benlee/Desktop/50_inliers.pcd")
	fil.set_negative(True)
	outlier_filtered = fil.filter()
	#pcl.save(outlier_filtered,"/home/benlee/Desktop/50_ioutliers.pcd")
	return inliter_filtered, outlier_filtered

def read_pcd():
	print("read pcd start")
	profile_np = np.load('/home/benlee/Desktop/pcd_data/pptk_save.npy')
	profile_pcd = pcl.load("/home/benlee/Desktop/pcd_data/pocket.pcd")

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

def check_inf(pc_data):
	point_size = len(pc_data)
	modified_pc = []
	for i in range(0, point_size):
		temp = pc_data[i]
		changed_arr = np.where(temp == np.inf, 0, temp)

		modified_pc.append(list(changed_arr))
		pc_data[i] = changed_arr 	
	modified_pc = np.array(modified_pc, dtype=np.float32)

	return modified_pc

def filter_remove():
	profile_np, profile_pcd = read_pcd() 
	ori_pc_np = check_inf(profile_np)

	# point cloud numpy, pcd read
	ori_pc = pcl.PointCloud(ori_pc_np) #numpy to pcl
	print("original size :", ori_pc.size)
	ori_pc = do_smoothihg(ori_pc)
	print("smoothing size :", ori_pc.size)

	visualize(ori_pc)

	# pcl.save(ori_pc_np,"/home/benlee/Desktop/ori_cloud.pcd")

	pass_filtered_pc = do_pass_through_filtering(ori_pc)

	pcl.save(pass_filtered_pc,"/home/benlee/Desktop/pass_filtered.pcd")
	print("passfilter size :", pass_filtered_pc.size)

	inlier_pc, outlier_pc =do_statistical_outlier_filter(pass_filtered_pc, 50, 1.0)

	voxel_filtered = do_voxel_grid_downsampling(inlier_pc, 0.05) #514,823
	print("voxelfilter size :", voxel_filtered.size)
	#visualize(cloud_filtered)

	#statistical outlier filter result save

	#visualize(voxel_filtered)
	final_pc = pcl.PointCloud(voxel_filtered) #numpy to pcl

	cloud_new = pcl_helper.pcl_to_ros(final_pc)

	#pointcloud XYZ to ROS PointCloud2 msg
	pub.publish(cloud_new)

	

if __name__=="__main__":
	rospy.init_node('cmm_test', anonymous=True)
	pub = rospy.Publisher("cmm_profile", PointCloud2, queue_size=1)
	filter_remove()
	rospy.spin()