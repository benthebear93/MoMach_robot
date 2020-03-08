#include <string>
#include <algorithm>
#include <time.h>
#include <iostream>
#include "mscl/mscl.h"
#include "gyro_test.h"
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
        int delta_angle = gyro_val * 0.1;
        current_angle = current_angle + delta_angle;
        return current_angle;
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
                int gy_r = 0;
                int gy_p = 0;
                int gy_y = 0;
                // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
                marker.action = visualization_msgs::Marker::ADD;
                //get all the packets that have been collected, with a timeout of 500 milliseconds
                try{
                    mscl::MipDataPackets packets = node.getDataPackets(500);
                    for(mscl::MipDataPacket packet : packets)
                    {
                        cout << "Packet Received: ";
                        mscl::MipDataPoints data = packet.data();
                        mscl::MipDataPoint dataPoint;
                        for(unsigned int itr = 0; itr < data.size(); itr++)
                        {
                            dataPoint = data[itr];

                            cout << dataPoint.channelName() << ": ";
                            cout << dataPoint.as_string() << " ";
                            std_msgs::Float32 myData;
                            string x = dataPoint.channelName();
                            uint32_t hash = const_hash(x.c_str());
                            switch(hash){
                                case const_hash("scaledGyroX"):                                 
                                    myData.data = dataPoint.as_float();
                                    myData.data = gyro_to_angle(myData.data, gy_r);
                                    gyro_x_pub.publish(myData);
                                    break;
                                case const_hash("scaledGyroY"):                                 
                                    myData.data = dataPoint.as_float();
                                    gyro_y_pub.publish(myData);
                                    break;
                                case const_hash("scaledGyroZ"):                                 
                                    myData.data = dataPoint.as_float();
                                    gyro_z_pub.publish(myData);
                                    break;
                                case const_hash("scaledAccelX"):                                 
                                    myData.data = dataPoint.as_float();
                                    acc_x_pub.publish(myData);
                                    break;
                                case const_hash("scaledAccelY"):                                 
                                    myData.data = dataPoint.as_float();
                                    acc_y_pub.publish(myData);
                                    break;
                                case const_hash("scaledAccelZ"):                                 
                                    myData.data = dataPoint.as_float();
                                    acc_z_pub.publish(myData);
                                    break;
                            }//switch
                            if(!dataPoint.valid())
                            {
                                cout << "[Invalid] ";
                            }
                        }
                        cout << endl;
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