#include <ros.h>
#include <std_msgs/Float64.h>
#define STEP_P 8
#define DIR_P 9
#define stepsPerRevolution 3200
#define f LOW
#define b HIGH

ros::NodeHandle nh;

float distance = 0;
float s_switch;

void switch_button( const std_msgs::Float64& msg){
  s_switch = msg.data;
  if(s_switch == 1){
    nh.loginfo("start");
  }else{
    nh.loginfo("stop");
  }
}

std_msgs::Float64 z_dis;
ros::Subscriber<std_msgs::Float64> sub("button", switch_button);
ros::Publisher pub("my_topic", &z_dis);


void steper_move(int speed_val, int dir){
    digitalWrite(DIR_P, dir); //set dir c
    for (int i = 0; i < stepsPerRevolution; i++){
      if(dir==HIGH){
        distance = distance + i*0.0000625;
      }else{
        distance = distance - i*0.0000625;
      }
      digitalWrite(STEP_P, HIGH);
      delayMicroseconds(speed_val);
      digitalWrite(STEP_P, LOW);
      delayMicroseconds(speed_val);
    }
}

void stepper_stop(int speed_val, int dir){
    nh.loginfo("stop");
}

void setup() {
  pinMode(STEP_P,OUTPUT);
  pinMode(DIR_P,OUTPUT);
  nh.advertise(pub);
  nh.subscribe(sub);
}

void loop() {
  z_dis.data = distance;
  pub.publish(&z_dis);
  if(s_switch == 1){
     steper_move(30, b); //speed = delay Ms
  }else{
     stepper_stop(0, b); //speed = delay Ms
  }
  nh.spinOnce();
  delay(10);
}
