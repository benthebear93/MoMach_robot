/****************************
Written by Haegu Lee 21/09/20
****************************/
#ifndef MOMACH_TF_H
#define MOMACH_TF_H
#include "ros/ros.h"
#include "MoMach_ros/pos_stream.h"
#include "MoMach_ros/vector_tf.h"
#include "MoMach_ros/triple_tf.h"

#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>

#include <cstring>
#include <iostream>
#include "std_msgs/Float32.h"
#include "std_msgs/Int32.h"
#include <assert.h>
#include <vector>
#include "typeinfo"

class Momach_tf{
    public:
        Momach_tf();
        ~Momach_tf();
        MoMach_ros::vector_tf v_tf;
        MoMach_ros::triple_tf tri_tf;
        std_msgs::Float32 error;
        const float offset[3] = {10.0f, 10.0f, 10.0f};
    private:
        bool init();
        ros::NodeHandle nh_;
        ros::Publisher vector_pub_;
        ros::Publisher error_pub_;
        ros::Subscriber pos_sub_;
        ros::Subscriber laser_sub_;
        void LaserCallback(const std_msgs::Float32 & msg);
        void PosCallback(const MoMach_ros::pos_stream & msg);
};
#endif