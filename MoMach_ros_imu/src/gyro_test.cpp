#include <string>
#include <algorithm>
#include <time.h>
#include <iostream>
#include "mscl/mscl.h"
#include "gyro_test.h"
#include <cmath>
#include <chrono>
#include "Eigen/Eigen"
#include "Eigen/Geometry" 
#include <tf/transform_datatypes.h>
#include <tf/transform_broadcaster.h>
using namespace std;

const string COM_PORT = "/dev/ttyACM0";
const int spin_rate = 5;
static double convertor_g2a  = 9.80665;    // for linear_acceleration (g to m/s^2)
static double convertor_d2r  = M_PI/180.0; // for angular_velocity (degree to radian)
static double convertor_r2d  = 180.0/M_PI; // for easy understanding (radian to degree)
static double convertor_ut2t = 1000000;    // for magnetic_field (uT to Tesla)
static double convertor_c    = 1.0;   

static constexpr uint32_t const_hash(const char *p) {
    return *p ? static_cast<uint32_t>(*p) + 33 * const_hash(p + 1) :  5381;
}

Eigen::Quaterniond euler2Quaternion(const double roll, const double pitch, const double yaw){
    Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitZ());

    Eigen::Quaterniond q = yawAngle * pitchAngle * rollAngle; //zyx
    Eigen::Matrix3d rotationMatrix =q.matrix();
    return q;
}

namespace MoMach_Imu{
    MoMach_Imu::MoMach_Imu(){
    }
    MoMach_Imu::~MoMach_Imu(){
    }

    float MoMach_Imu::gyro_integration(float gyro_val, float current_angle){
        float delta_angle = gyro_val * 0.01; //topic pub 100hz 
        current_angle = current_angle + delta_angle;
        return current_angle;
    }
    float *MoMach_Imu::RP_calculate(float acc[]){
        float roll = atan2(acc[1], acc[2]);
        float pitch = atan2((acc[0]), sqrt(acc[1]*acc[1] + acc[2]*acc[2]));
        RPY_buff[0] = roll;
        RPY_buff[1] = pitch;
        return RPY_buff;
    } 
    void MoMach_Imu::init(){
        
        ros::NodeHandle n;
        gyro_r_pub = n.advertise<std_msgs::Float32>("gyro_r", 100);
        gyro_p_pub = n.advertise<std_msgs::Float32>("gyro_p", 100);
        gyro_y_pub = n.advertise<std_msgs::Float32>("gyro_y", 100);

        acc_roll_pub = n.advertise<std_msgs::Float32>("acc_roll", 100);
        acc_pitch_pub = n.advertise<std_msgs::Float32>("acc_pitch", 100);

        filt_roll_pub = n.advertise<std_msgs::Float32>("filt_roll", 100);
        filt_pitch_pub = n.advertise<std_msgs::Float32>("filt_pitch", 100);

        marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

        imu_data_pub_= n.advertise<sensor_msgs::Imu>("imu/data_raw", 1);
        imu_mag_pub_ = n.advertise<sensor_msgs::MagneticField>("imu/mag", 1);
        imu_temperature_pub_= n.advertise<std_msgs::Float64>("imu/temperature", 1);
        rpy_publisher_ = n.advertise<geometry_msgs::Vector3Stamped>(
            ros::names::resolve("imu") + "/rpy/filtered", 1);
        rpy_publisher = n.advertise<geometry_msgs::Vector3>("rpy_origin", 1000);
        
    }

    void *MoMach_Imu::publish_imu(float orientation[], float acc_buff[], float angluar_buff[]){
        ros::Time now = ros::Time::now();
        imu_data_msg.header.stamp = now;
        imu_data_msg.header.frame_id = frame_id_;
        imu_data_msg.orientation.x = orientation[0];
        imu_data_msg.orientation.y = orientation[1];
        imu_data_msg.orientation.z = orientation[2];
        imu_data_msg.orientation.w = orientation[3];

        imu_data_msg.linear_acceleration.x     =  acc_buff[0] * convertor_g2a;
        imu_data_msg.linear_acceleration.y     = -acc_buff[1] * convertor_g2a;
        imu_data_msg.linear_acceleration.z     = -acc_buff[2] * convertor_g2a;

        imu_data_msg.angular_velocity.x     =  angluar_buff[0];
        imu_data_msg.angular_velocity.y     = -angluar_buff[1];
        imu_data_msg.angular_velocity.z     = -angluar_buff[2];

        imu_magnetic_msg.magnetic_field.x =  0;
        imu_magnetic_msg.magnetic_field.y = 0;
        imu_magnetic_msg.magnetic_field.z = 0;
        imu_temperature_msg.data = 0;   
        
        imu_mag_pub_.publish(imu_magnetic_msg);
        imu_data_pub_.publish(imu_data_msg);
        imu_temperature_pub_.publish(imu_temperature_msg);
    }

    void MoMach_Imu::run(){
        try
        {
            mscl::Connection connection = mscl::Connection::Serial("/dev/ttyACM0", 115200);
            mscl::InertialNode node(connection);
            bool success = node.ping();
            node.setToIdle();

            mscl::MipChannels ahrsImuChs;
            ahrsImuChs.push_back(mscl::MipChannel(mscl::MipTypes::CH_FIELD_SENSOR_SCALED_ACCEL_VEC, mscl::SampleRate::Hertz(100)));
            ahrsImuChs.push_back(mscl::MipChannel(mscl::MipTypes::CH_FIELD_SENSOR_SCALED_GYRO_VEC, mscl::SampleRate::Hertz(100)));
            node.setActiveChannelFields(mscl::MipTypes::CLASS_AHRS_IMU, ahrsImuChs);
            node.enableDataStream(mscl::MipTypes::CLASS_AHRS_IMU);
            init();

            ROS_INFO("Setting spin rate to <%d>", spin_rate);
            ros::Rate r(spin_rate); // Rate in Hz 10Hz = 
            time_t start = time(NULL);
            while(ros::ok())
            {
                try{
                    mscl::MipDataPackets packets = node.getDataPackets(0); //10 ms = 0.01s = 100hz
                    time_t end = time(NULL);
                    cout << "end :" << end << endl;
                    cout << "start :" << start << endl;
                    float gyro_temp = gyro_buff[2];
                    float ten_second = float(end-start);
                    //cout << "ten :" << ten_second << endl;
                    // if(ten_second >10){
                    //     cout << "time :" << ten_second << "  "<< gyro_temp/ten_second << "deg/sec" << endl;
                    // }
                    for(mscl::MipDataPacket packet : packets)
                    {   
                        chrono::system_clock::time_point StartTime = chrono::system_clock::now();
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
                                    angular_buff[0] = dataPoint.as_float();                  
                                    Data_gr.data = dataPoint.as_float()*convertor_r2d; 
                                    gyro_buff[0] = gyro_integration(Data_gr.data, gyro_buff[0]);
                                    quat_buff[0] = gyro_buff[0] * convertor_d2r;
                                    break;
                                case const_hash("scaledGyroY"):       
                                    angular_buff[1] = dataPoint.as_float();                             
                                    Data_gp.data = dataPoint.as_float()*convertor_r2d;
                                    gyro_buff[1] = gyro_integration(Data_gp.data, gyro_buff[1]);
                                    quat_buff[1] = gyro_buff[1] * convertor_d2r;
                                    break;
                                case const_hash("scaledGyroZ"): 
                                    angular_buff[2] = dataPoint.as_float();                                   
                                    Data_gy.data = dataPoint.as_float()*convertor_r2d;
                                    gyro_buff[2] = gyro_integration(Data_gy.data, gyro_buff[2]);
                                    quat_buff[2] = gyro_buff[2] * convertor_d2r;
                                    break;
                                //accel data
                                case const_hash("scaledAccelX"):    
                                    acc_buff[0] = dataPoint.as_float();
                                    break;
                                case const_hash("scaledAccelY"):                                 
                                    acc_buff[1] = dataPoint.as_float();  
                                    break;
                                case const_hash("scaledAccelZ"):                                 
                                    acc_buff[2] = dataPoint.as_float(); 
                                    break;
                            }//switch
                            a = RP_calculate(acc_buff);
                            RPY_buff[0] = a[0] *convertor_r2d;
                            RPY_buff[1] = a[1] *convertor_r2d;

                            Data_gr.data = gyro_buff[0] *convertor_d2r;
                            Data_gp.data = gyro_buff[1] *convertor_d2r;
                            Data_gy.data = gyro_buff[2] *convertor_d2r;

                            if(RPY_buff[0] > 0){
                                Data_acc_roll.data =-180 + RPY_buff[0];
                                a[0] = (-180.0 + RPY_buff[0]) * convertor_d2r;
                            }else{
                                Data_acc_roll.data = 180 + RPY_buff[0];
                                a[0] = (180.0 + RPY_buff[0]) * convertor_d2r;
                            } //acc's roll data  --> deg
                            Data_acc_pitch.data = RPY_buff[1];
                            //acc's pitch data --> deg
                            a[1] = RPY_buff[1] * convertor_d2r;
                            if(!dataPoint.valid())
                            {
                                cout << "[Invalid] ";
                            }
                        }
                        gyro_r_pub.publish(Data_gr);
                        gyro_p_pub.publish(Data_gp);
                        gyro_y_pub.publish(Data_gy);
                        ////gyro angle data -> deg

                        acc_roll_pub.publish(Data_acc_roll);
                        acc_pitch_pub.publish(Data_acc_pitch);
                        ///acc_roll, pitch data -> deg

                        filt_roll_pub.publish(Data_fil_roll);
                        filt_pitch_pub.publish(Data_fil_pitch);
                        //filtred roll, pitch data -> deg
                        
                        Eigen::Quaterniond q =euler2Quaternion(a[0], a[1], quat_buff[2]);
                        Orientation[0] = q.x();
                        Orientation[1] = q.y();
                        Orientation[2] = q.z();
                        Orientation[3] = q.w();
                        publish_imu(Orientation, acc_buff, angular_buff);
                        
                        tf::Transform transform;
                        tf::Quaternion q_tf;
                        q_tf.setRPY(quat_buff[0], quat_buff[1], quat_buff[2]);
                        transform.setOrigin(tf::Vector3(0.0, 0.0, 0.0));
                        transform.setRotation(q_tf);
                        tf_broadcaster_.sendTransform(
                            tf::StampedTransform(transform,
                                                imu_data_msg.header.stamp,
                                                "lordIMU",
                                                "imu"));
                        tf::Matrix3x3 m_(q_tf);
                        double r_,p_,y_;
                        m_.getRPY(r_,p_,y_);
                        geometry_msgs::Vector3 rpy;
                        rpy.x = r_;
                        rpy.y = p_;
                        rpy.z = y_;
                        rpy_publisher.publish(rpy);
                        // chrono::system_clock::time_point EndTime = chrono::system_clock::now();
                        // chrono::duration<double> DefaultSec = EndTime - StartTime;
                        // chrono::milliseconds mill = chrono::duration_cast<chrono::milliseconds>(EndTime - StartTime);
                        // cout <<"Test time :" << DefaultSec.count() << " default " << endl;
                        // ////put into sensor_msgs/Imu format/////
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
