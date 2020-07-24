#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int16.h>
#include <pos_stream.h>

#define Pulse 9
#define Dir 8

ros::NodeHandle nh;

long delay_Micros =3; // Set value
long currentMicros = 0; long previousMicros = 0;
int rev_flag = 0;
float tool_z = 0;
float controller_z = 0;

std_msgs::Float32 z_tool;
ros::Publisher pub("tool_z", &z_tool);

void GetControl_z(const MoMach_ros::pos_stream& Controller_data){
  nh.loginfo("CW");
  controller_z = Controller_data.pos_z;
  tool_z = controller_z;
}

ros::Subscriber<MoMach_ros::pos_stream> sub("pos_stream", GetControl_z);

void setup()
{
  Serial.begin(57600);
  pinMode(Pulse,OUTPUT);
  pinMode(Dir,OUTPUT);
  nh.subscribe(sub);
}
void loop()
{

  if(tool_z<0){
    digitalWrite(Dir,HIGH);
    digitalWrite(Pulse,HIGH);
    delayMicroseconds(delay_Micros); //Set Value
    digitalWrite(Pulse,LOW);
    delayMicroseconds(delay_Micros); //Set Value
  }
  else if(tool_z>0){
    nh.loginfo("CCW");
    digitalWrite(Dir,LOW);
    digitalWrite(Pulse,HIGH);
    delayMicroseconds(delay_Micros); //Set Value
    digitalWrite(Pulse,LOW);
    delayMicroseconds(delay_Micros); //Set Value
  }
  else{
     nh.loginfo("STOP");
      digitalWrite(Pulse,HIGH);
      delayMicroseconds(delay_Micros); //Set Value
      digitalWrite(Pulse,HIGH);
      delayMicroseconds(delay_Micros); //Set Value
  }
  nh.spinOnce();
}
