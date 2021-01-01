#include "MoMach_ros/momach_tf.h"

#define OFFSET 20
using namespace std;

const static float feed = 50.0;
MoMach_ros::vector_tf v_tf;
MoMach_ros::triple_tf tri_tf;
std_msgs::Int32 error;

Momach_tf::Momach_tf() // init func
{
    ROS_INFO("Momach_tf Node init");
    auto ret = Momach_tf::init();
    ROS_ASSERT(ret);
}
Momach_tf::~Momach_tf() // destroy
{
    ros::shutdown();
}

bool Momach_tf::init()
{
  ROS_INFO("tf broadcasting start");
  vector_pub_    = nh_.advertise<MoMach_ros::vector_tf>("vector", 100); 
  error_pub_     = nh_.advertise<std_msgs::Float32>("error", 100); 
  
  pos_sub_    = nh_.subscribe("pos_stream", 1, &Momach_tf::PosCallback, this);
  laser_sub_ = nh_.subscribe("mid_laser", 1, &Momach_tf::LaserCallback, this);

  return true;
}

void Momach_tf::LaserCallback(const std_msgs::Float32 & msg){
  //position_z = msg.data;
}

void Momach_tf::PosCallback(const MoMach_ros::pos_stream & msg){
  static tf2_ros::TransformBroadcaster br;
  static tf2_ros::TransformBroadcaster br2;

  geometry_msgs::TransformStamped tool_tip;
  geometry_msgs::TransformStamped laser;
  
  tool_tip.header.stamp = ros::Time::now();
  tool_tip.header.frame_id = "world";
  tool_tip.child_frame_id = "tool_tip";
  tool_tip.transform.translation.x = 0.001 * msg.pos_x;
  tool_tip.transform.translation.y = 0.001 * msg.pos_y;
  tool_tip.transform.translation.z = 0.001 * msg.pos_z;
  // tool tip translation set up

  laser.header.stamp = ros::Time::now();
  laser.header.frame_id = "world";
  laser.child_frame_id = "laser";
  laser.transform.translation.x = (0.001 * msg.pos_x) + offset[0];
  laser.transform.translation.y = (0.001 * msg.pos_y) + offset[1];
  laser.transform.translation.z = (0.001 * msg.pos_z) + offset[2];
  // laser translation coordinate set up

  tf2::Quaternion q;
  q.setRPY(0, 0, 0);
  tool_tip.transform.rotation.x = q.x();
  tool_tip.transform.rotation.y = q.y();
  tool_tip.transform.rotation.z = q.z();
  tool_tip.transform.rotation.w = q.w();
  // tool tip rotation setup

  laser.transform.rotation.x = q.x();
  laser.transform.rotation.y = q.y();
  laser.transform.rotation.z = q.z();
  laser.transform.rotation.w = q.w();
  // laser rotation setup

  tri_tf.tf_x = laser.transform.translation.x*1000;
  tri_tf.tf_y = laser.transform.translation.y*1000;
  tri_tf.tf_z = laser.transform.translation.z*1000;

  /*v_tf.data.push_back(tri_tf);
  cout <<"first z : " << v_tf.data[0].tf_z << endl;
  cout <<"first x : " << v_tf.data[0].tf_x << endl;
  cout << "            " << endl;
  cout <<"tool x : " << tool_tip.transform.translation.x << endl;
  vector_pub_.publish(v_tf);
  error.data = v_tf.data[0].tf_z;
  error_pub_.publish(error);*/
  br.sendTransform(tool_tip);
  br2.sendTransform(laser);
}


int main(int argc, char **argv)
{
  //Initiate ROS
  ros::init(argc, argv, "subscribe_and_publish");
  //Create an object of class SubscribeAndPublish that will take care of everything
  Momach_tf momach_tf;
  ros::spin();
  return 0;
}