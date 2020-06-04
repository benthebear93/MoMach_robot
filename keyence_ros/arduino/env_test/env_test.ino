#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int16.h>
#include <avr/dtostrf.h> 
#define STEP_P 8
#define DIR_P 9
#define stepsPerRevolution 3200
#define f LOW
#define b HIGH
#define thr_vel 200;
ros::NodeHandle nh;

double z_init = 21.0;
float z_measure = 0;
float s_switch;
float s_goback;

float error = 0;

float p_val = 0;
float i_val = 0;
float Kp = 0;
float Ki = 0;
float vel = 0;

void Switch_button( const std_msgs::Float64& swt1){
  nh.loginfo("swt bu");
  s_switch = swt1.data;
  if(s_switch == 1){
    nh.loginfo("start switch");
  }else{
    nh.loginfo("stop switch");
    z_init = 0.0;
  }
}

void Switch_go_back( const std_msgs::Float64& swt2){
  nh.loginfo("swt go back");
  s_goback = swt2.data;
  if(s_goback == 1){
    nh.loginfo("go");
  }else{
    nh.loginfo("back");
  }
}

void Error_pid(const std_msgs::Float32& laser){
  nh.loginfo("error pid");
  z_measure = laser.data;
  error = z_init - z_measure;
  p_val = error*Kp;
  i_val = i_val + error*Ki;
  vel = p_val + i_val;
}

std_msgs::Float64 z_dis;
std_msgs::Float32 check_laser;
ros::Subscriber<std_msgs::Float64> sub2("go_back_button", Switch_go_back);
ros::Subscriber<std_msgs::Float64> sub("start_button", Switch_button);
ros::Subscriber<std_msgs::Float32> sub3("mid_laser", Error_pid);
ros::Publisher pub("z_distacne", &z_dis);
ros::Publisher pub2("error", &check_laser);

void backnforth(){
  if(s_goback == 1){
    if(s_switch == 1){
      stepper_move(200, f); //speed = delay Ms
    }else{
      stepper_stop(0, f); //speed = delay Ms
    }
  }
  else{
    if(s_switch == 1){
      stepper_move(200, b); //speed = delay Ms
    }else{
      stepper_stop(0, b); //speed = delay Ms
    }
  }
}

void pid_control(){
  if(vel < 0){ //go up
    stepper_move(200+vel, f);
  }
  else{ // go down
    stepper_move(200+vel, b);
  }
}
void stepper_move(int speed_val, int dir){
    digitalWrite(DIR_P, dir);//set dir c 
    for (int i = 0; i < stepsPerRevolution; i++){
      if(dir==LOW){
        z_init = z_init + 0.000625;
      }else{
        z_init = z_init - 0.000625;
      }
      digitalWrite(STEP_P, HIGH);
      delayMicroseconds(speed_val);
      digitalWrite(STEP_P, LOW);
      delayMicroseconds(speed_val);
      
      if(i%5 == 0){
        z_dis.data = z_init;
        pub.publish(&z_dis);
        check_laser.data = error;
        pub2.publish(&check_laser);
      }
    }
}

void stepper_stop(int speed_val, int dir){
}

void setup() {
  pinMode(STEP_P,OUTPUT);
  pinMode(DIR_P,OUTPUT);
  nh.advertise(pub);
  nh.subscribe(sub);
  nh.subscribe(sub2);
  nh.subscribe(sub3);
  nh.advertise(pub2);
}

void loop() {
  backnforth();
  nh.spinOnce();
  //delay(10);
}
