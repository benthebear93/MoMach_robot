#!/usr/bin/env python2
import sys
import time

import geometry_msgs.msg
import rospy
import tf2_ros
import tf_conversions
from MoMach_ros.msg import pos_stream
from std_msgs.msg import Float32, Float32MultiArray, Float64, Int16, Int32

import numpy as np

FEED = 100 #100mm/min =  1.6667
FEED_MM_S = 1.6667
OFF_SET = 74 #mm

class Laser_Sensor:
    
    def __init__(self, feed):
        self.feed = feed #(mm/s)
        self.pos_x = 0
        self.pos_y = 0
        self.pos_z = 0
        self.laser_data = []
    

def laser_save(laser_data):

    '''callback function subscribe laser data
    save laser data to the buffer'''

    Laser_Sensor.laser_data.append(laser_data.data)
    saved_data = Float32MultiArray(data = Laser_Sensor.laser_data)
    Laser_Sensor.pos_z =laser_data.data
    pub.publish(saved_data)


def tool_pos_update(con_odom):

    '''tool pos update from controller (pos_stream.cpp)
    controller odometry = con_odom x axis'''
    br = tf2_ros.TransformBroadcaster()
    br2 = tf2_ros.TransformBroadcaster()
    laser = geometry_msgs.msg.TransformStamped()
    tool_pos = geometry_msgs.msg.TransformStamped()

    laser.header.stamp = rospy.Time.now()
    tool_pos.header.stamp = rospy.Time.now()

    laser.header.frame_id = "map"
    laser.child_frame_id = "laser_tip"

    tool_pos.header.frame_id = "map"
    tool_pos.child_frame_id = "tool_tip"

    laser.transform.translation.x = (con_odom.pos_x)*0.001 + OFF_SET
    laser.transform.translation.y = 0*0.001
    laser.transform.translation.z = (Laser_Sensor.pos_z)


    tool_pos.transform.translation.x = (con_odom.pos_x)*0.001
    tool_pos.transform.translation.y = 0*0.001
    tool_pos.transform.translation.z = 0.001*(con_odom.pos_z) + float(sys.argv[1]) #- float(sys.argv[1]))  #(float(sys.argv[1]))*0.001

    q = tf_conversions.transformations.quaternion_from_euler(0 ,0, 0)
    laser.transform.rotation.x = q[0]
    laser.transform.rotation.y = q[1]
    laser.transform.rotation.z = q[2]
    laser.transform.rotation.w = q[3]

    tool_pos.transform.rotation.x = q[0]
    tool_pos.transform.rotation.y = q[1]
    tool_pos.transform.rotation.z = q[2]
    tool_pos.transform.rotation.w = q[3]
    
    br.sendTransform(laser)
    br2.sendTransform(tool_pos)

    
if __name__=='__main__':
    Laser_Sensor = Laser_Sensor(FEED_MM_S)
    rospy.init_node('tool_simulation', anonymous=True)
    rospy.Subscriber('mid_laser', Float32, laser_save)
    rospy.Subscriber('pos_stream', pos_stream, tool_pos_update)
    pub = rospy.Publisher('saved_laser_data', Float32MultiArray, queue_size =100)

    rospy.spin()


if(auto_correction_flag == 0){
    if(tool_pos.transform.translation.x == laser_buffer.front().x ) auto_correction_flag = 1;
}
else{
    error = laser_buffer.points[i] -tool_pos.transform.roation.z;
    error_pub.publish(error);
}
