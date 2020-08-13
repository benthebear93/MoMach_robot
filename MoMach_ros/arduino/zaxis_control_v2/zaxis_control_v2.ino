#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int32.h>
#include <math.h>
#include <stdlib.h>
#include <pos_stream.h>
#define stepsPerRevolution 25000
#define mmResolution 5
#define Pulse 9
#define Dir 8
#define f LOW
#define b HIGH
ros::NodeHandle nh;

//gloabl
int g_keyboard_input = 0;
int g_con_z = 0;
int g_con_temp_z = 0;
int g_tool_z = 0;
int g_total_keyinput = 0;

//flag
bool flag_init = false;

std_msgs::Int32 tool_pos;
ros::Publisher pub("tool_pos_msg", &tool_pos);

std_msgs::Int32 difference_z;
ros::Publisher pub2("difference_z", &difference_z);

void StepperMove(int speed_val) 
{
  digitalWrite(Pulse, HIGH);
  delayMicroseconds(speed_val);
  digitalWrite(Pulse, LOW);
  delayMicroseconds(speed_val);
} //motor input

void ZaxisKeyboard(const std_msgs::Int32& zaxis_input)
{
  g_keyboard_input = zaxis_input.data;
  g_total_keyinput = g_total_keyinput + g_keyboard_input;
  if(g_keyboard_input==1)nh.loginfo("here");
  else nh.loginfo("here2");
} //keyboard interrupt for control z axis with keyboard (while robot running)

void GetControlZ(const MoMach_ros::pos_stream& Controller_data)
{
  if(flag_init == false)
  {
    g_con_z = Controller_data.pos_z;
    g_con_temp_z = g_con_z;
    g_tool_z =  g_con_z;
    flag_init = true;
  }
  else
  {
    g_con_z = Controller_data.pos_z;
  }
  ControlMode(g_con_temp_z, g_con_z, 10);
}

void ToolMove(int difference, int dir, int speed)
{
  int num_of_rotation = mmResolution*abs(difference);
  if(dir == 1)
  {
    digitalWrite(Dir, b);
    for(int i = 0; i < num_of_rotation; i++)
    {
      //nh.loginfo("dir :1, +0.2 ");
      if(i == (num_of_rotation-1)) g_tool_z = g_tool_z + num_of_rotation*0.2;
      StepperMove(speed);
    } 
  }
  else
  {
    digitalWrite(Dir, f);
    for(int i = 0; i < num_of_rotation; i++)
    {
      //nh.loginfo("dir :-1, -0.2 ");
      if(i == (num_of_rotation-1)) g_tool_z = g_tool_z - num_of_rotation*0.2;
      StepperMove(speed);
    } 
  }
  tool_pos.data = g_tool_z;
  pub.publish(&tool_pos);
}

void ControlMode(int prev_con_z, int curr_con_z, int speed)
{
  int diff_z = curr_con_z - prev_con_z;
  if(diff_z !=0) // z motor moving by controller
  {
    if(curr_con_z == g_tool_z)
    {
      if(diff_z > 0)
      {
        ToolMove(diff_z, 1, speed);
        //nh.loginfo("diff > 0 ");
      }
      else if(diff_z < 0)
      {
        ToolMove(diff_z, 0, speed);
        //nh.loginfo("diff > 0 ");
      }
    }
    else
    {
      nh.loginfo("not same ");
      int total_diff = diff_z - g_total_keyinput;//-1, 10 = 9 -1 -10 = -11 
      difference_z.data = total_diff;
      pub2.publish(&difference_z);
      if(total_diff > 0)ToolMove(total_diff, 1, speed);
      else if(total_diff < 0)ToolMove(total_diff, 0, speed);
      g_total_keyinput = 0;
      //g_keyboard_input =
    }
  } 
  else // z motor not moving
  {
    if(g_keyboard_input > 0)
    {
      nh.loginfo("input > 0 ");
      ToolMove(g_keyboard_input, b, speed);
    }
    else if(g_keyboard_input < 0)
    {
      nh.loginfo("input < 0 ");
      ToolMove(g_keyboard_input, f, speed);
    }
  }
  g_con_temp_z = curr_con_z;
  g_keyboard_input = 0;
}
ros::Subscriber<MoMach_ros::pos_stream> sub("pos_stream", GetControlZ);
ros::Subscriber<std_msgs::Int32> sub2("zaxis_key_input", ZaxisKeyboard);

void setup()
{
  pinMode(Pulse,OUTPUT);
  pinMode(Dir,OUTPUT);
  nh.subscribe(sub);
  nh.subscribe(sub2);
  nh.advertise(pub);
  nh.advertise(pub2);
}

void loop()
{
  nh.spinOnce();
}
