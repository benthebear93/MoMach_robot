#ifndef _GYRO_TEST_H
#define _GYRO_TEST_H

#include <cstdio>
#include <iostream>
#include <time.h>

#include "ros/ros.h"
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

#include "mscl/mscl.h"

namespace MoMach_Imu{
    
    class MoMach_Imu{
        public:
            MoMach_Imu();
            ~MoMach_Imu();

        void run();

        private:
            float ahrs_acc_x;
            float ahrs_acc_y;
            float ahrs_acc_z;
            float ahrs_gyro_x;
            float ahrs_gyro_y;
            float ahrs_gyro_z;
    };
}
#endif