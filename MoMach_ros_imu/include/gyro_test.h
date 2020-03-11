#ifndef _GYRO_TEST_H
#define _GYRO_TEST_H

#include <cstdio>
#include <iostream>
#include <time.h>
#include <vector>
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
            float gyro_integration(float gyro_val, float current_angle);
            float rad2deg(float rad);
            float *RP_calculate(float acc_v[]);
            float *Complentary(float acc_rpy[], float gyro_rpy[]);
            void init_node();

        private:
            float gyro_buff[3] = {0,}; //deg
            float acc_buff[3] = {0, }; //rad 
            float RPY_buff[3] = {0, }; //deg
            float filitered_rpy[3] = {0,};
            std_msgs::Float32 Data_gr;
            std_msgs::Float32 Data_gp;
            std_msgs::Float32 Data_gy;
            std_msgs::Float32 Data_ar;
            std_msgs::Float32 Data_ap;
            std_msgs::Float32 Data_ay;

            std_msgs::Float32 Data_acc_roll;
            std_msgs::Float32 Data_acc_pitch;
            std_msgs::Float32 Data_fil_roll;
            std_msgs::Float32 Data_fil_pitch;
            //std_msgs::Float32 Data_accz;


            // ros::Publisher acc_x_pub;
            // ros::Publisher acc_y_pub;
            // ros::Publisher acc_z_pub;
            ros::Publisher gyro_r_pub;
            ros::Publisher gyro_p_pub;
            ros::Publisher gyro_y_pub;
            ros::Publisher marker_pub;

            ros::Publisher acc_roll_pub;
            ros::Publisher acc_pitch_pub;
            ros::Publisher filt_roll_pub;
            ros::Publisher filt_pitch_pub;

    };
}
#endif