#include <ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int16.h>
#define STEP_P 8
#define DIR_P 9
#define stepsPerRevolution 3200
#define f LOW
#define b HIGH
int thr_val = 100;
ros::NodeHandle nh;

float error =0; //pid feedback error
float z_t = 10.0; // tool z
float z_m = 0; // measurment z 

int s_switch = 0;
int s_goback = 0;
int s_reset = 0;
float error_ = 0;

float p_val = 0;
float i_val = 0;
int pid_val = 0;
float kp = 10;
float ki = 0;
float dv = 0;
bool dir = 0;
int vel = 0;

int debug_flag =0;

std_msgs::Float32 z_tool;
std_msgs::Float32 error_val;
ros::Publisher pub("z_distacne", &z_tool);
ros::Publisher pub2("error", &error_val);


void Switch_button( const std_msgs::Int16& swt1){
  s_switch = swt1.data;
  if(s_switch == 1){
    nh.loginfo("1.start switch");
  }else{
    nh.loginfo("0.stop switch");
  }
}

void Switch_go_back( const std_msgs::Int16& swt2){
  s_goback = swt2.data;
  if(s_goback == 1){
    nh.loginfo("1.go");
  }else{
    nh.loginfo("0.back");
  }
}

void Switch_reset( const std_msgs::Int16& swt3){
  nh.loginfo("reset function");
  s_reset = swt3.data;
  if(s_reset == 1){
    nh.loginfo("1.start");
  }else{
    nh.loginfo("0.reset");
  }
}

void Laser_data(const std_msgs::Float32& laser){
  //nh.loginfo("laser callback?");
  z_m = laser.data;
}

void PID_Control(){
  nh.loginfo("control?");
  if(s_switch==1){
    error = z_t - z_m;
    if(error !=0){
      for (int i = 0; i < stepsPerRevolution; i++){
        error = z_t - z_m;
        if(error < 0){ // tool need to go up!
          nh.loginfo("go up?");
          digitalWrite(DIR_P, f);//forward(LOW)
          dir = 0;
        }
        else{ // tool need to go down!
          nh.loginfo("go down?");
          digitalWrite(DIR_P, b);//backward(HIGH)
          dir = 1;
        }
        if(dir==0){
          digitalWrite(DIR_P, f);//forward(LOW)
          z_t = z_t + 0.000625;
        }else{
          digitalWrite(DIR_P, b);//backward(HIGH)
          z_t = z_t - 0.000625;
        }
        //if(error > 0)p_val = kp*error;
        //else p_val = kp*(-error);
        //i_val = i_val +ki*error*cycle_time;
        //d_val = kd*(error-error_pre)/cycle_time;
        pid_val = p_val; //+i_val+d_val
        vel = thr_val - pid_val;
        digitalWrite(STEP_P, HIGH);
        delayMicroseconds(50);
        digitalWrite(STEP_P, LOW);
        delayMicroseconds(50);
        if(i%2 == 0){
          pubtest();
        }
      }
    }
  }else{
    stepper_stop(0, f); //speed = delay Ms
  }
}
void pubtest(){
  z_tool.data = z_t;
  pub.publish(&z_tool);
  error_val.data = vel;
  pub2.publish(&error_val);
}

ros::Subscriber<std_msgs::Int16> sub("start_button", Switch_button);
ros::Subscriber<std_msgs::Int16> sub2("go_back_button", Switch_go_back);
ros::Subscriber<std_msgs::Int16> sub4("reset_button", Switch_reset);
ros::Subscriber<std_msgs::Float32> sub3("mid_laser", Laser_data);

void backnforth(){ // for the test only
  if(s_reset == 1){
    if(s_goback == 0){
      nh.loginfo("0.straight");
      stepper_move(100, f); //speed = delay Ms
    }else{
      nh.loginfo("1.back");
      stepper_move(100, b); //speed = delay Ms
    }
  }
  else{
    stepper_stop(0, b); //speed = delay Ms
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
  nh.advertise(pub2);
    
  nh.subscribe(sub);
  nh.subscribe(sub2);
  nh.subscribe(sub3);
  nh.subscribe(sub4);
}

void loop() {
  if(s_reset ==1){
    backnforth();
    debug_flag = 0;
  }
  else{
    if(debug_flag==0){
      nh.loginfo("stop reset and start control");
      nh.loginfo("start PID control func");
      debug_flag = 1;
    }
    PID_Control();
  }
  nh.spinOnce();
}
