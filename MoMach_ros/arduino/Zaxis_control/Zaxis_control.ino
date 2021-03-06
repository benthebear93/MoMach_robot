#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>
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
int g_diff_z = 0;
int g_pos_z = 0;
int g_auto_z = 0;
int g_zaxis_keyboard = 0;

std_msgs::Int32 tool_pos;
ros::Publisher pub("tool_pos_msg", &tool_pos);

void StepperMove(int speed_val) 
{
  digitalWrite(Pulse, HIGH);
  delayMicroseconds(speed_val);
  digitalWrite(Pulse, LOW);
  delayMicroseconds(speed_val);
} //motor input

void ZaxisKeyboard(const std_msgs::Int32& zaxis_input)
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
  ControlMode(g_temp_z, tool_z, 10);
} // get z axis value from Delta controllerpos_stream

ros::Subscriber<MoMach_ros::pos_stream> sub("pos_stream", GetControlZ);
ros::Subscriber<std_msgs::Int32> sub2("zaxis_key_input", ZaxisKeyboard);

void setup()
{
  pinMode(Pulse,OUTPUT);
  pinMode(Dir,OUTPUT);
  nh.subscribe(sub);
  nh.subscribe(sub2);
  nh.advertise(pub);
}

void ToolMove(int difference, int keyboard_input, int dir, int speed_val)
{
  digitalWrite(Dir, dir);
  for(int i = 0; i < mmResolution*abs(difference+keyboard_input); i++)
  {
    g_pos_z = g_pos_z + 0.2;
    nh.loginfo("forward");
    StepperMove(speed_val);
  } 
}

void ControlMode(int prev_z, int curr_z, int speed_val)
{
  int z_key_input = 0;
  g_diff_z = curr_z - prev_z;
  if(g_diff_z > 0)
  {
    ToolMove(g_diff_z, g_zaxis_keyboard, b, speed_val);
  }
  else if(g_diff_z < 0)
  {
    ToolMove(g_diff_z, g_zaxis_keyboard, f, speed_val);
  }
  else
  {
    if(g_zaxis_keyboard > 0)
    {
      nh.loginfo("stop");
      digitalWrite(Dir, b);
      for(int i = 0; i < mmResolution*abs(g_zaxis_keyboard); i++)
      {
        g_auto_z = g_pos_z + 0.2;
        nh.loginfo("keyboard +1");
        StepperMove(speed_val);
      }
    }
    else if(g_zaxis_keyboard < 0)
    {
      digitalWrite(Dir, f);
      for(int i = 0; i < mmResolution*abs(g_zaxis_keyboard); i++)
      {
        g_auto_z = g_pos_z - 0.2;
        nh.loginfo("keyboard -1");
        StepperMove(speed_val);
      }
    }
  }
  tool_pos.data = g_pos_z;
  pub.publish(&tool_pos);
  g_temp_z = curr_z;
  g_zaxis_keyboard = 0;
} // mimic Delta z axis motor motion based on z axis data. 

void loop()
{
  nh.spinOnce();
}
