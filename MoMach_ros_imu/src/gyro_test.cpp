#include <string>
#include <algorithm>
#include <time.h>
#include <iostream>
#include "mscl/mscl.h"
#include "gyro_test.h"
using namespace std;

namespace MoMach_Imu{
    MoMach_Imu::MoMach_Imu(){

    }
    MoMach_Imu::~MoMach_Imu(){

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
            cout << "Node Information: " << endl;
            cout << "Model Name: " << node.modelName() << endl;
            cout << "Model Number: " << node.modelNumber() << endl;
            cout << "Serial: " << node.serialNumber() << endl;
            cout << "Firmware: " << node.firmwareVersion().str() << endl << endl;
            node.setToIdle();
            mscl::MipChannels ahrsImuChs;
            cout << "1";
            ahrsImuChs.push_back(mscl::MipChannel(mscl::MipTypes::CH_FIELD_SENSOR_SCALED_ACCEL_VEC, mscl::SampleRate::Hertz(100)));
            cout << "acc";
            ahrsImuChs.push_back(mscl::MipChannel(mscl::MipTypes::CH_FIELD_SENSOR_SCALED_GYRO_VEC, mscl::SampleRate::Hertz(100)));
            cout << "gyro"<<endl;
            node.setActiveChannelFields(mscl::MipTypes::CLASS_AHRS_IMU, ahrsImuChs);
            cout << "set active"<<endl;
            node.enableDataStream(mscl::MipTypes::CLASS_AHRS_IMU);
            cout << "enable"<<endl;
            cout << "ros start"<< endl;

            ros::Publisher acc_x_pub = n.advertise<std_msgs::Float32>("acc_x", 100);

            int max_rate = 10;
            int spin_rate = min(2 * max_rate, 1000);
            ROS_INFO("Setting spin rate to <%d>", spin_rate);
            ros::Rate r(spin_rate); // Rate in Hz
            while(ros::ok())
            {
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

                            if(dataPoint.channelName() == "scaledAccelX"){
                                myData.data = dataPoint.as_float();
                                acc_x_pub.publish(myData);
                            }
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