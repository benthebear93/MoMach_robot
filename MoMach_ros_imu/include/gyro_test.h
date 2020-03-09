#ifndef _GYRO_TEST_H
#define _GYRO_TEST_H

#include <cstdio>
#include <iostream>
#include <time.h>

#include "ros/ros.h"
#include "ros/console.h"
#include "sensor_msgs/NavSatFix.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Vector3.h"
#include "nav_msgs/Odometry.h"
#include "std_msgs/Int8.h"
#include "std_msgs/Int16MultiArray.h"
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include <visualization_msgs/Marker.h>
#include "mscl/mscl.h"

namespace MoMach_Imu{
    
    class MoMach_Imu{
        public:
            MoMach_Imu();
            ~MoMach_Imu();
            void run();
            float gyro_to_angle(float gyro_val, float current_angle);
            float rad2deg(float rad);

        private:
            float gyx_temp = 0.;
            float gyy_temp = 0.;
            float gyz_temp = 0.;
            float accx_temp = 0.;
            float accy_temp = 0.;
            float accz_temp = 0.;

            std_msgs::Float32 Data_gx;
            std_msgs::Float32 Data_gy;
            std_msgs::Float32 Data_gz;
            std_msgs::Float32 Data_ax;
            std_msgs::Float32 Data_ay;
            std_msgs::Float32 Data_az;


            ros::Publisher acc_x_pub;
            ros::Publisher acc_y_pub;
            ros::Publisher acc_z_pub;
            ros::Publisher gyro_x_pub;
            ros::Publisher gyro_y_pub;
            ros::Publisher gyro_z_pub;
            ros::Publisher marker_pub;
            ros::NodeHandle *nh;
    };
}
#endif