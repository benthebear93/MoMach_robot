#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int16.h>
#define STEP_P 8
#define DIR_P 9
#define stepsPerRevolution 3200
#define f LOW
#define b HIGH
int thr_val = 200;
ros::NodeHandle nh;

float error =0; //pid feedback error
float z_t = 21.0; // tool z
float z_m = 0; // measurment z 

float s_switch = 0;
float s_goback = 0;
float error_ = 0;

float p_val = 0;
float i_val = 0;

float kp = 0;
float ki = 0;
float dv = 0;
bool dir = 0;

std_msgs::Float32 z_tool;
std_msgs::Float32 error_val;
ros::Publisher pub("z_distacne", &z_tool);
ros::Publisher pub2("error", &error_val);


void Switch_button( const std_msgs::Int16& swt1){
  nh.loginfo("swt bu");
  s_switch = swt1.data;
  if(s_switch == 1){
    nh.loginfo("start switch");
  }else{
    nh.loginfo("stop switch");
    z_t = 0.0;
  }
}

void Switch_go_back( const std_msgs::Int16& swt2){
  nh.loginfo("swt go back");
  s_goback = swt2.data;
  if(s_goback == 1){
    nh.loginfo("go");
  }else{
    nh.loginfo("back");
  }
}

void PID_control(const std_msgs::Float32& laser){
  error = z_t - z_m;
  if(error < 0){ // tool need to go up!
    digitalWrite(DIR_P, f);//forward
  }
  else{ // tool need to go down!
    digitalWrite(DIR_P, b);//backward
  }
  p_val = error*kp;
  dv = p_val;
  thr_val = thr_val - p_val;
  if(error != 0){
    for (int i = 0; i < stepsPerRevolution; i++){
      if(dir==LOW){
        z_t = z_t + 0.000625;
      }else{
        z_t = z_t - 0.000625;
      }
      digitalWrite(STEP_P, HIGH);
      delayMicroseconds(thr_val);
      digitalWrite(STEP_P, LOW);
      delayMicroseconds(thr_val);
      if(i%5 == 0){
        pubtest();
      }
    }   
  } 
}

void pubtest(){
  z_tool.data = z_t;
  pub.publish(&z_tool);
  error_val.data = error;
  pub2.publish(&error_val);
}
ros::Subscriber<std_msgs::Int16> sub("start_button", Switch_button);
ros::Subscriber<std_msgs::Int16> sub2("go_back_button", Switch_go_back);
ros::Subscriber<std_msgs::Float32> sub3("mid_laser", PID_control);

void backnforth(){ // for the test only
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

void stepper_move(int speed_val, int dir){
  digitalWrite(DIR_P, dir);//set dir c 
  for (int i = 0; i < stepsPerRevolution; i++){
    if(dir==LOW){
      z_t = z_t + 0.000625;
    }else{
      z_t = z_t - 0.000625;
    }
    digitalWrite(STEP_P, HIGH);
    delayMicroseconds(speed_val);
    digitalWrite(STEP_P, LOW);
    delayMicroseconds(speed_val);
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
  //backnforth();
  nh.spinOnce();
  delay(10);
}
