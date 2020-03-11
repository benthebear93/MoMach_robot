#include <string>
#include <algorithm>
#include <time.h>
#include <iostream>
#include "mscl/mscl.h"
#include "gyro_test.h"
#include <cmath>
#include <chrono>
using namespace std;
const string COM_PORT = "/dev/ttyACM0";
const int spin_rate = 5;
double sample_time;
static constexpr uint32_t const_hash(const char *p) {
    return *p ? static_cast<uint32_t>(*p) + 33 * const_hash(p + 1) :  5381;
}


namespace MoMach_Imu{
    MoMach_Imu::MoMach_Imu(){

    }
    MoMach_Imu::~MoMach_Imu(){

    }

    float MoMach_Imu::gyro_integration(float gyro_val, float current_angle){
        // cout << "pre time :" << pre_time <<endl;

        // cout << "current time :" << curr_time <<endl;
        // float dt = float(curr_time-pre_time);
        // cout << "dt : " << dt << endl;
        float delta_angle = gyro_val * 0.01; //topic pub 100hz 
        current_angle = current_angle + delta_angle;
        return current_angle;
    }

    float MoMach_Imu::rad2deg(float rad){
        float ans = (rad*180.)/M_PI;
        return ans;
    }   

    float *MoMach_Imu::RP_calculate(float acc[]){
        float roll = atan2(acc[1], acc[2]);
        float pitch = atan2((acc[0]), sqrt(acc[1]*acc[1] + acc[2]*acc[2]));
        RPY_buff[0] = roll;
        RPY_buff[1] = pitch;
        return RPY_buff;
    } 
    float *MoMach_Imu::Complentary(float acc_rpy[], float gyro_rpy[]){
        const float alpha = 0.996;
        for(int i =0; i < 2; i++){
            filitered_rpy[i] = alpha*(gyro_rpy[i] * 0.01 + filitered_rpy[i]) + (1-alpha) * acc_rpy[i];
        }
        return filitered_rpy;
    }

    void MoMach_Imu::init_node(){
        
        ros::NodeHandle n;
        // acc_x_pub = n.advertise<std_msgs::Float32>("acc_x", 100);
        // acc_y_pub = n.advertise<std_msgs::Float32>("acc_y", 100);
        // acc_z_pub = n.advertise<std_msgs::Float32>("acc_z", 100);
        gyro_r_pub = n.advertise<std_msgs::Float32>("gyro_r", 100);
        gyro_p_pub = n.advertise<std_msgs::Float32>("gyro_p", 100);
        gyro_y_pub = n.advertise<std_msgs::Float32>("gyro_y", 100);

        acc_roll_pub = n.advertise<std_msgs::Float32>("acc_roll", 100);
        acc_pitch_pub = n.advertise<std_msgs::Float32>("acc_pitch", 100);
        filt_roll_pub = n.advertise<std_msgs::Float32>("filt_roll", 100);
        filt_pitch_pub = n.advertise<std_msgs::Float32>("filt_pitch", 100);

        marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

    }
    void MoMach_Imu::run(){

        try
        {
            //create a SerialConnection with the COM port
            mscl::Connection connection = mscl::Connection::Serial("/dev/ttyACM0", 115200);
            //create an InertialNode with the connection
            mscl::InertialNode node(connection);
            bool success = node.ping();
            node.setToIdle();

            mscl::MipChannels ahrsImuChs;
            ahrsImuChs.push_back(mscl::MipChannel(mscl::MipTypes::CH_FIELD_SENSOR_SCALED_ACCEL_VEC, mscl::SampleRate::Hertz(100)));
            ahrsImuChs.push_back(mscl::MipChannel(mscl::MipTypes::CH_FIELD_SENSOR_SCALED_GYRO_VEC, mscl::SampleRate::Hertz(100)));
            node.setActiveChannelFields(mscl::MipTypes::CLASS_AHRS_IMU, ahrsImuChs);
            node.enableDataStream(mscl::MipTypes::CLASS_AHRS_IMU);
            //pub_topics 
            init_node();
            //visual marker
            uint32_t shape = visualization_msgs::Marker::CUBE;

            //rate setting
            ROS_INFO("Setting spin rate to <%d>", spin_rate);
            ros::Rate r(spin_rate); // Rate in Hz 10Hz = 
            while(ros::ok())
            {
                visualization_msgs::Marker marker;
                 // Set the frame ID and timestamp.  See the TF tutorials for information on these.
                marker.header.frame_id = "/my_frame";
                marker.header.stamp = ros::Time::now();
                marker.ns = "basic_shapes";
                marker.id = 0;
                marker.type = shape;
                // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
                marker.action = visualization_msgs::Marker::ADD;
                ROS_INFO("CHECK");
                try{
                    mscl::MipDataPackets packets = node.getDataPackets(0); //10 ms = 0.01s = 100hz
                    // time_t end = time(NULL);
                    // cout <<  "end:" << end << endl;
                    // float ten_second = float(end-start);
                    // if(ten_second > 1.0){
                    //     float gyro_temp = gyro_buff[2];
                    //     cout <<"gyro_temp :" << gyro_temp << "gyro_changed :" << gyro_buff[2]<<endl;
                    //     start = time(NULL);
                    // }
                    // cout << gyro_buff[2] << endl;
                    chrono::system_clock::time_point StartTime = chrono::system_clock::now();
                    for(mscl::MipDataPacket packet : packets)
                    {   
                        mscl::MipDataPoints data = packet.data();
                        mscl::MipDataPoint dataPoint;
                        for(unsigned int itr = 0; itr < data.size(); itr++)
                        {
                            dataPoint = data[itr];
                            string x = dataPoint.channelName();
                            uint32_t hash = const_hash(x.c_str());
                            switch(hash){
                                //gyro datas
                                case const_hash("scaledGyroX"):                        
                                    Data_gr.data = rad2deg(dataPoint.as_float()); 
                                    gyro_buff[0] = gyro_integration(Data_gr.data, gyro_buff[0]);
                                    // Data_gr.data = gyro_buff[0];
                                    // gyro_r_pub.publish(Data_gr);
                                    break;
                                case const_hash("scaledGyroY"):                                 
                                    Data_gp.data = rad2deg(dataPoint.as_float());
                                    gyro_buff[1] = gyro_integration(Data_gp.data, gyro_buff[1]);
                                    // Data_gp.data = gyro_buff[1];
                                    // gyro_p_pub.publish(Data_gp);
                                    break;
                                case const_hash("scaledGyroZ"):                                 
                                    Data_gy.data = rad2deg(dataPoint.as_float());
                                    gyro_buff[2] = gyro_integration(Data_gy.data, gyro_buff[2]);
                                    // Data_gy.data = gyro_buff[2];
                                    // gyro_y_pub.publish(Data_gy);
                                    break;
                                //accel data
                                case const_hash("scaledAccelX"):    
                                    acc_buff[0] = dataPoint.as_float();
                                    // Data_ax.data = acc_buff[0];                                   
                                    // acc_x_pub.publish(Data_ax);
                                    break;
                                case const_hash("scaledAccelY"):                                 
                                    acc_buff[1] = dataPoint.as_float();  
                                    // Data_ay.data = acc_buff[1];                                  
                                    // acc_y_pub.publish(Data_ay);
                                    break;
                                case const_hash("scaledAccelZ"):                                 
                                    acc_buff[2] = dataPoint.as_float(); 
                                    // Data_az.data = acc_buff[2];                                   
                                    // acc_z_pub.publish(Data_az);
                                    break;
                            }//switch
                            chrono::system_clock::time_point EndTime = chrono::system_clock::now();
                            chrono::duration<double> DefaultSec = EndTime - StartTime;
                            chrono::microseconds micro =chrono::duration_cast<chrono::microseconds>(EndTime - StartTime);
                            cout << "Test() 함수를 수행하는 걸린 시간 : " << DefaultSec.count() << " default" << endl;
                            float * a = RP_calculate(acc_buff);
                            RPY_buff[0] = rad2deg(a[0]);
                            RPY_buff[1] = rad2deg(a[1]);

                            Data_gr.data = gyro_buff[0];
                            gyro_r_pub.publish(Data_gr);
                            Data_gp.data = gyro_buff[1];
                            gyro_p_pub.publish(Data_gp);
                            Data_gy.data = gyro_buff[2];
                            gyro_y_pub.publish(Data_gy);

                            Data_acc_roll.data = RPY_buff[0];
                            acc_roll_pub.publish(Data_acc_roll);
                            Data_acc_pitch.data = RPY_buff[1];
                            acc_pitch_pub.publish(Data_acc_pitch);

                            float * b = Complentary(RPY_buff, gyro_buff);
                            filitered_rpy[0] = b[0];
                            filitered_rpy[0] = b[1];
                            Data_fil_roll.data = filitered_rpy[0];
                            Data_fil_pitch.data = filitered_rpy[1];
                            filt_roll_pub.publish(Data_fil_roll);
                            filt_pitch_pub.publish(Data_fil_pitch);
                            float c = 180.0 - RPY_buff[0];
                            float d = 180.0 - RPY_buff[1];
                            if(!dataPoint.valid())
                            {
                                cout << "[Invalid] ";
                            }
                        }
                    }
                }
                catch(mscl::Error_Connection){
                    ROS_ERROR("Device Disconnected");
                    break;
                }
                //ros::spinOnce(); // take care of service requests.
                r.sleep(); 
            }
        }
        catch (mscl::Error& e){
            ROS_FATAL("Error: %s", e.what());
        }
    } //run()
}//namespace