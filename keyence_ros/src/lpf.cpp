#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Int32.h"
//#include "keyence_ros/keyence_laser.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

class LPF
{
    public:
        LPF();
        ~LPF();
        bool init();
        void LowPassFilter();
    private:
        ros::NodeHandle nh_;

        ros::Publisher lk_dis_pub_;
        ros::Publisher lj_dis_pub_;

        ros::Subscriber lk_sub_;
        ros::Subscriber lj_sub_;

        double pre_val_;
        double curr_val_;
        double avg_curr_val_;
        double ts_;
        double tau_;

        void lk_callback(const std_msgs::Float32::ConstPtr& msg);
        void lj_callback(const std_msgs::Float32::ConstPtr& msg);
}; 

LPF::LPF()
{
    ROS_INFO("LPF Node Init");
    auto ret = init();
    ROS_ASSERT(ret);
}
LPF::~LPF()
{
    ros::shutdown();
}
bool LPF::init()
{
    pre_val_     = 0;
    curr_val_     = 0;
    avg_curr_val_ = 0;
    ts_           = 0.005;
    tau_          = 0.001;
    lk_dis_pub_   = nh_.advertise<std_msgs::Float32>("lf_lk_dis", 10);
    lj_dis_pub_   = nh_.advertise<std_msgs::Float32>("lf_lj_dis", 10);

    lk_sub_  = nh_.subscribe("LK_laser", 10, &LPF::lk_callback, this);
    lj_sub_  = nh_.subscribe("mid_laser", 10, &LPF::lj_callback, this);
    return true;
}

void LPF::lk_callback(const std_msgs::Float32::ConstPtr& msg)
{
    std_msgs::Float32 distance;
    distance.data = pre_val_;
    lk_dis_pub_.publish(distance);
    curr_val_ = msg->data;
}
void LPF::lj_callback(const std_msgs::Float32::ConstPtr& msg)
{
    std_msgs::Float32 distance;
    distance.data = pre_val_;
    lj_dis_pub_.publish(distance);
    curr_val_ = msg->data;
}

void LPF::LowPassFilter()
{
    cout << "curr_val_ : " << curr_val_<<endl;
    cout << "pre_val_ : " << pre_val_ << endl;
    curr_val_ = (tau_*pre_val_+curr_val_*ts_)/(tau_ + ts_);
    pre_val_ = curr_val_;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "lpf");
  LPF lk_lpf;
  LPF lj_lpf;

  ros::Rate loop_rate(100);
  while(ros::ok())
  {
    lk_lpf.LowPassFilter();
    lj_lpf.LowPassFilter();
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}