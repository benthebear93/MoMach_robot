#include "gyro_test.h"
#include <ros/ros.h>

int main(int argc, char **argv){
    ros::init(argc, argv, "Imu_acc_pub");
    MoMach_Imu::MoMach_Imu test;
    test.run();
    ros::shutdown();
    return 0;
}