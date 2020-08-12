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
int g_zaxis_keyboard = 0;

std_msgs::Int16 tool_pos;
ros::Publisher difference_check("tool_pos_msg", &tool_pos);

void StepperMove(int speed_val) 
{
  digitalWrite(Pulse, HIGH);
  delayMicroseconds(speed_val);
  digitalWrite(Pulse, LOW);
  delayMicroseconds(speed_val);
} //motor input

void ZaxisKeyboard(const std_msgs::Int16& zaxis_input)
{
  g_zaxis_keyboard = zaxis_input.data;
  if(g_zaxis_keyboard==1)nh.loginfo("here");
  else nh.loginfo("here2");
} //keyboard interrupt for control z axis with keyboard (while robot running)

void GetControlZ(const MoMach_ros::pos_stream& Controller_data)
{
  int tool_z = 0; 
  if(flag_init == 0) // initialize first z value
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
} // get z axis value from Delta controller

ros::Subscriber<MoMach_ros::pos_stream> sub("pos_stream", GetControlZ);
ros::Subscriber<std_msgs::Int16> sub2("zaxis_key_input", ZaxisKeyboard);

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
  int z_key_input = 0;
  g_theta_z = curr_z - prev_z;
  if(g_theta_z > 0)
  {
    digitalWrite(Dir, b);
    for(int i = 0; i < mmResolution*abs(g_theta_z+g_zaxis_keyboard); i++)
    {
      g_pos_z = g_pos_z + 0.2;
      z_key_input = g_pos_z;
      nh.loginfo("forward");
      StepperMove(speed_val);
    }
  }
  else if(g_theta_z < 0)
  {
    digitalWrite(Dir, f);
    for(int i = 0; i < mmResolution*abs(g_theta_z+g_zaxis_keyboard); i++)
    {
      g_pos_z = g_pos_z - 0.2;
      z_key_input = g_pos_z;
      nh.loginfo("backward");
      StepperMove(speed_val);
    }
  }
  else
  {
    if(g_zaxis_keyboard > 0)
    {
      digitalWrite(Dir, b);
      for(int i = 0; i < mmResolution*abs(g_zaxis_keyboard); i++)
      {
        z_key_input = z_key_input + 0.2;
        nh.loginfo("keyboard +1");
        StepperMove(speed_val);
      }
    }
    else if(g_zaxis_keyboard < 0)
    {
      digitalWrite(Dir, f);
      for(int i = 0; i < mmResolution*abs(g_zaxis_keyboard); i++)
      {
        z_key_input = z_key_input - 0.2;
        nh.loginfo("keyboard -1");
        StepperMove(speed_val);
      }
    }
  }
  tool_pos.data = z_key_input;
  difference_check.publish(&tool_pos);
  g_temp_z = curr_z;
  g_zaxis_keyboard = 0;
} // mimic Delta z axis motor motion based on z axis data. 

void loop()
{
  nh.spinOnce();
}
