import pcl
import numpy as np
import pptk

profile_load = np.load('/home/benlee/Desktop/pptk_save.npy')
#infcheck = np.isinf(profile_load)
print(profile_load)
final = []
#print("len : ", len(infcheck))
for i in range(0, (929600)):
	check =profile_load[i]
	#print(check)
	changed_arr = np.where(check == np.inf, 0, check)
	final.append(list(changed_arr))
	profile_load[i] = changed_arr
final = np.array(final, dtype=np.float32)

pc = pcl.PointCloud(final)


#print("pcl.pointcloud", pc, type(pc))
#pcl.save(pc, 'pocket.pcd')


print("pcl.pointcloud form array", pc, type(pc))
print(pc)

#visualize 
#v = pptk.viewer(final)
#v.set(point_size = 0.00001)
