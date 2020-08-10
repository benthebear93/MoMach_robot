#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int16.h>
#include <pos_stream.h>
#include <math.h>
#include <stdlib.h>
#define stepsPerRevolution 25000
#define mmResolution 5
#define Pulse 9
#define Dir 8
#define f LOW
#define b HIGH
ros::NodeHandle nh;
int flag_init = 0;
int g_s_switch = 0;
int g_s_run = 0;

int g_temp_z = 0;
int g_theta_z = 0;
int g_pos_z = 0;

std_msgs::Int16 tool_pos;
ros::Publisher difference_check("tool_pos_msg", &tool_pos);

void StepperMove(int speed_val)
{
  digitalWrite(Pulse, HIGH);
  delayMicroseconds(speed_val);
  digitalWrite(Pulse, LOW);
  delayMicroseconds(speed_val);
}

void SwitchStop( const std_msgs::Int16& swt1)
{
  g_s_switch = swt1.data;
  if(g_s_switch == 1){
    nh.loginfo("1.stop");
  }else{
    nh.loginfo("0.run");
  }
}

void GetControlZ(const MoMach_ros::pos_stream& Controller_data)
{
  int tool_z = 0; 
  if(flag_init == 0)
  {
    tool_z = Controller_data.pos_z;
    g_temp_z = tool_z;
    g_pos_z = tool_z;
    flag_init = 1;
  }
  else
  {
    tool_z = Controller_data.pos_z;
    g_pos_z = tool_z;
  }
  ControlMode(g_temp_z, tool_z, 3);
}

ros::Subscriber<MoMach_ros::pos_stream> sub("pos_stream", GetControlZ);
ros::Subscriber<std_msgs::Int16> sub2("stop", SwitchStop);

void setup()
{
  pinMode(Pulse,OUTPUT);
  pinMode(Dir,OUTPUT);
  nh.subscribe(sub);
  nh.subscribe(sub2);
  nh.advertise(difference_check);
}

void ControlMode(int prev_z, int curr_z, int speed_val)
{
  g_theta_z = curr_z - prev_z;
  if(g_theta_z > 0)
  {
    digitalWrite(Dir, f);
    for(int i = 0; i < mmResolution*abs(g_theta_z); i++)
    {
      g_pos_z = g_pos_z + 0.2;
      nh.loginfo("forward");
      StepperMove(speed_val);
    }
  }
  else if(g_theta_z < 0)
  {
    digitalWrite(Dir, b);
    for(int i = 0; i < mmResolution*abs(g_theta_z); i++)
    {
      g_pos_z = g_pos_z - 0.2;
      nh.loginfo("backward");
      StepperMove(speed_val);
    }
  }
  else
  {
  }
  tool_pos.data = g_pos_z;
  difference_check.publish(&tool_pos);
  g_temp_z = curr_z;
}

void loop()
{
  nh.spinOnce();
}
