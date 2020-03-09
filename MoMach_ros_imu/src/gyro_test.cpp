#include <string>
#include <algorithm>
#include <time.h>
#include <iostream>
#include "mscl/mscl.h"
#include "gyro_test.h"
#include <cmath>
using namespace std;
  static constexpr uint32_t const_hash(const char *p) {
    return *p ? static_cast<uint32_t>(*p) + 33 * const_hash(p + 1) :  5381;
  }
namespace MoMach_Imu{
    MoMach_Imu::MoMach_Imu(){

    }
    MoMach_Imu::~MoMach_Imu(){

    }
    float MoMach_Imu::gyro_to_angle(float gyro_val, float current_angle){
        float delta_angle = gyro_val * 0.01; //topic pub 100hz 
        current_angle = current_angle + delta_angle;
        return current_angle;
    }

    float MoMach_Imu::rad2deg(float rad){
        float ans = (rad*180.)/M_PI;
        return ans;
    }      
    void MoMach_Imu::run(){

        ros::Time::init();
        ros::NodeHandle n;
        const string COM_PORT = "/dev/ttyACM0";
        cout << "is it working?" << endl;
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

            acc_x_pub = n.advertise<std_msgs::Float32>("acc_x", 100);
            acc_y_pub = n.advertise<std_msgs::Float32>("acc_y", 100);
            acc_z_pub = n.advertise<std_msgs::Float32>("acc_z", 100);
            gyro_x_pub = n.advertise<std_msgs::Float32>("gyro_x", 100);
            gyro_y_pub = n.advertise<std_msgs::Float32>("gyro_y", 100);
            gyro_z_pub = n.advertise<std_msgs::Float32>("gyro_z", 100);
            marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);
            uint32_t shape = visualization_msgs::Marker::CUBE;

            int max_rate = 10;
            int spin_rate = min(2 * max_rate, 1000);
            ROS_INFO("Setting spin rate to <%d>", spin_rate);
            ros::Rate r(spin_rate); // Rate in Hz
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
                //get all the packets that have been collected, with a timeout of 500 milliseconds
                try{
                    mscl::MipDataPackets packets = node.getDataPackets(10);
                    int flag = 0;
                    for(mscl::MipDataPacket packet : packets)
                    {   
                        cout << flag<< endl;
                        if(flag ==1 ){
                            cout<<gyx_temp<<endl;
                            cout<<gyy_temp<<endl;
                            cout<<gyz_temp<<endl;
                            cout<<accx_temp<<endl;
                            cout<<accy_temp<<endl;
                            cout<<accz_temp<<endl;
                        
                            break;
                        }
                        //cout << "Packet Received: ";
                        mscl::MipDataPoints data = packet.data();
                        mscl::MipDataPoint dataPoint;
                        for(unsigned int itr = 0; itr < data.size(); itr++)
                        {
                            flag = 1;
                            dataPoint = data[itr];
                            string x = dataPoint.channelName();
                            uint32_t hash = const_hash(x.c_str());
                            switch(hash){
                                case const_hash("scaledGyroX"):                        
                                    Data_gx.data = rad2deg(dataPoint.as_float());
                                    gyx_temp = gyro_to_angle(Data_gx.data, gyx_temp);
                                    Data_gx.data = gyx_temp;
                                    gyro_x_pub.publish(Data_gx);
                                    break;
                                case const_hash("scaledGyroY"):                                 
                                    Data_gy.data = rad2deg(dataPoint.as_float());
                                    gyy_temp = gyro_to_angle(Data_gy.data, gyy_temp);
                                    Data_gy.data = gyy_temp;
                                    gyro_y_pub.publish(Data_gy);
                                    break;
                                case const_hash("scaledGyroZ"):                                 
                                    Data_gz.data = rad2deg(dataPoint.as_float());
                                    gyz_temp = gyro_to_angle(Data_gz.data, gyz_temp);
                                    Data_gz.data = gyz_temp;
                                    gyro_z_pub.publish(Data_gz);
                                    break;
                                case const_hash("scaledAccelX"):                                 
                                    Data_ax.data = rad2deg(dataPoint.as_float());
                                    accx_temp = gyro_to_angle(Data_ax.data, accx_temp);
                                    Data_ax.data = accx_temp;
                                    acc_x_pub.publish(Data_ax);
                                    break;
                                case const_hash("scaledAccelY"):                                 
                                    Data_ay.data = rad2deg(dataPoint.as_float());
                                    accy_temp = gyro_to_angle(Data_ay.data, accy_temp);
                                    Data_ay.data = accy_temp;
                                    acc_y_pub.publish(Data_ay);
                                    break;
                                case const_hash("scaledAccelZ"):                                 
                                    Data_az.data = rad2deg(dataPoint.as_float());
                                    accz_temp = gyro_to_angle(Data_az.data, accz_temp);
                                    Data_az.data = accz_temp;
                                    acc_z_pub.publish(Data_az);
                                    break;
                            }//switch
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
                ros::spinOnce(); // take care of service requests.
                r.sleep(); 
            }
        }
        catch (mscl::Error& e){
            ROS_FATAL("Error: %s", e.what());
        }
    } //run()
}//namespace