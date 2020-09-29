#include <ros/ros.h>
#include "std_msgs/Float32.h"
#include "std_msgs/Int32.h"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/TransformStamped.h>
#include "MoMach_ros/pos_stream.h"
#include "MoMach_ros/vector_tf.h"
#include "MoMach_ros/triple_tf.h"
#include <iostream>
#include <ros/ros.h>
#include <assert.h>
#include <vector>
#include "typeinfo"
#define OFFSET 20
using namespace std;

const static float feed = 50.0;
MoMach_ros::vector_tf v_tf;
MoMach_ros::triple_tf tri_tf;
std_msgs::Int32 error;

class LaserSubPubHandle
{
public:
  float position_z;

  LaserSubPubHandle()
  {
    vector_pub = nh.advertise<MoMach_ros::vector_tf>("vector", 100);
    error_pub = nh.advertise<std_msgs::Int32>("error", 100);
    laser_sub = nh.subscribe("mid_laser", 10, &LaserSubPubHandle::laser_callback, this);
    pos_sub = nh.subscribe("pos_stream", 10, &LaserSubPubHandle::pos_callback, this);
    position_z = 0;
  }

  void laser_callback(const std_msgs::Float32 & msg){
    position_z = msg.data;
  }

  void pos_callback(const MoMach_ros::pos_stream & msg){
    static tf2_ros::TransformBroadcaster br;
    static tf2_ros::TransformBroadcaster br2;

    geometry_msgs::TransformStamped tool_tip;
    geometry_msgs::TransformStamped laser;
    
    tool_tip.header.stamp = ros::Time::now();
    tool_tip.header.frame_id = "world";
    tool_tip.child_frame_id = "tool_tip";
    tool_tip.transform.translation.x = 0.001 * msg.pos_x;
    tool_tip.transform.translation.y = 0.0;
    tool_tip.transform.translation.z = 0.001 * msg.pos_z + 19.5;

    laser.header.stamp = ros::Time::now();
    laser.header.frame_id = "world";
    laser.child_frame_id = "laser";
    laser.transform.translation.x = 0.001 * msg.pos_x;
    laser.transform.translation.y = 0.0;
    laser.transform.translation.z = position_z;

    tf2::Quaternion q;
    q.setRPY(0, 0, 0);
    tool_tip.transform.rotation.x = q.x();
    tool_tip.transform.rotation.y = q.y();
    tool_tip.transform.rotation.z = q.z();
    tool_tip.transform.rotation.w = q.w();

    laser.transform.rotation.x = q.x();
    laser.transform.rotation.y = q.y();
    laser.transform.rotation.z = q.z();
    laser.transform.rotation.w = q.w();

    tri_tf.tf_x = laser.transform.translation.x*1000;
    tri_tf.tf_y = laser.transform.translation.y*1000;
    tri_tf.tf_z = laser.transform.translation.z*1000;

    v_tf.data.push_back(tri_tf);
    cout <<"first z : " << v_tf.data[0].tf_z << endl;
    cout <<"first x : " << v_tf.data[0].tf_x << endl;
    cout << "            " << endl;
    cout <<"tool x : " << tool_tip.transform.translation.x << endl;
    //assert(v_tf.data[0].tf_x < 0.0);
    /*if(tool_tip.transform.translation.x == v_tf.data[0].tf_x){
      cout << "arrived" << endl;
      cout << " laser x :" << laser.transform.translation.x << endl;
      cout << "tool x :" << tool_tip.transform.translation.x << endl;
      v_tf.data.erase(v_tf.data.begin());
    }*/
    vector_pub.publish(v_tf);
    error.data = v_tf.data[0].tf_z;
    error_pub.publish(error);
    br.sendTransform(tool_tip);
    br2.sendTransform(laser);
  }

private:
  ros::NodeHandle nh; 
  ros::Publisher vector_pub;
  ros::Publisher error_pub;
  ros::Subscriber laser_sub;
  ros::Subscriber pos_sub;
};


int main(int argc, char **argv)
{
  //Initiate ROS
  ros::init(argc, argv, "subscribe_and_publish");
  //Create an object of class SubscribeAndPublish that will take care of everything
  LaserSubPubHandle LAERSubpub;
  ros::spin();
  return 0;
}