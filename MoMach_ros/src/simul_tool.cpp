#include <ros/ros.h>
#include "std_msgs/Float32.h"
#include "std_msgs/Int32.h"
#include <sensor_msgs/PointCloud2.h>
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

MoMach_ros::vector_tf v_tf;
MoMach_ros::triple_tf tri_tf;
std_msgs::Int32 error;

class LaserSubPubHandle
{
public:
  bool init_flag;
  int idx, tolerance;
  float tool_x, tool_y, tool_z;
  LaserSubPubHandle()
  {
    error_pub = nh.advertise<std_msgs::Int32>("error", 100);
    vector_sub = nh.subscribe("vector", 10, &LaserSubPubHandle::saved_callback, this);
    pos_sub = nh.subscribe("pos_stream", 10, &LaserSubPubHandle::pos_callback, this);
    init_flag = 0;
    tool_z = 0;
    tool_y = 0;
    tool_x = 0;
    idx = 0;
    tolerance = 30;
  }
    void pos_callback(const MoMach_ros::pos_stream & msg){
        //cout << "pos callback" <<endl;
        tool_x = msg.pos_x;
        tool_y = msg.pos_y;
        tool_z = msg.pos_z; // 0.001mm
  }
  void saved_callback(const MoMach_ros::vector_tf & v_tf){
      //cout << "saved callback" <<endl;
      if(tool_x -v_tf.data[0].tf_x >= tolerance && init_flag ==0){
        cout << "saved callback" <<endl;
        cout << "arrived" << endl;
        cout << " laser x :" << v_tf.data[0].tf_x  << endl;
        cout << "tool x :" << tool_x << endl;
        init_flag = 1;
      }
      cout << "tool_x : " << tool_x <<endl;
      cout << "tool_y : " << tool_y <<endl;
      cout << "tool_z : " << tool_z <<endl;
      cout << "v_tf x : " << v_tf.data[0].tf_x <<endl;

      if(init_flag ==1){
        cout << "idx : " << idx <<endl;
        cout << "laser z: " << v_tf.data[idx].tf_z <<endl;
        //cout << " error z :" << v_tf.data[idx].tf_z  << endl;
        error.data = v_tf.data[idx].tf_z;
        error_pub.publish(error);
        idx = idx + 1;
      }
  }
private:
  ros::NodeHandle nh; 
  ros::Subscriber vector_sub;
  ros::Publisher error_pub;
  ros::Subscriber pos_sub;
};


int main(int argc, char **argv)
{
  //Initiate ROS
  ros::init(argc, argv, "error_node");
  //Create an object of class SubscribeAndPublish that will take care of everything
  LaserSubPubHandle error_node;
  ros::spin();
  return 0;
}