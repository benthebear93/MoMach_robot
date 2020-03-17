# Path following system

##### Path following system without feedback.



Robot only moves x, y axis.

Moving velocity is fixed, but It's not possible to send a data to Monitoring system.

Let's consider the situation when robot moves without stop.

This way we only need to know that whether robot is moving x or y.

With help from IMU Sensor's accelerometer we might be able to distinguish robot's 

orientation. Additionally Robot's velocity too. 



### Tasks

1. Live Grid Map(창현's help)
2. Robot moving? or is it stopped? -> IMU sensor data check1
3. Robot moving x axis? or y axis? -> IMU sensor data check 2
4. Moving robot test. 



if 'robot' is moving

​	if 'robot' is moving x axis

​		while(imu_x> 0.3xxx || imu_x< -0.3xxx)

​			x = x + velocity * t

​	if 'robot' is moving y axis 

​		while(imu_y> 0.3xxx || imu_y < -0.3xxx)

​			y = y + velocity * t

) 