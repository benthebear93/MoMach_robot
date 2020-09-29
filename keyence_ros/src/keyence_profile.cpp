#include "ros/ros.h"
#include "std_msgs/Float32.h"
//#include "keyence_ros/keyence_laser.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
//pcl related
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/ros/conversions.h>
#include <pcl/io/pcd_io.h>
#include <pcl_ros/point_cloud.h>
#include <sensor_msgs/PointCloud2.h>
//define
#define KEYENCE "169.254.29.71"
#define PORT 24691
using namespace std;


int main(int argc, char**argv)
{
    pcl::PointCloud<pcl::PointXYZ> cloud;
    cloud.width = 16;
    cloud.height = 1;
    cloud.is_dense = false;
    cloud.points.resize (cloud.width * cloud.height);
    //init pcl format
    std_msgs::Float32 mid_laser;
    ros::init(argc, argv, "keyence_get_profile");
    ros::NodeHandle nh;
    ros::Publisher mid_laser_pub = nh.advertise<std_msgs::Float32>("mid_laser" ,100);
    ros::Rate loop_rate(1000); //1000Hz = 0.05sec
    //ros node & publisher init
    int sockfd;
    struct sockaddr_in servaddr,cliaddr;
    unsigned char LJV_GetMeasurementValues[] = { 0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0xF0, 0x00, \
                                        0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, \
                                        0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    size_t LJV_GetMeasurementValues_s = sizeof(LJV_GetMeasurementValues);  //get size

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(KEYENCE);
    servaddr.sin_port=htons(PORT);
    int a = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));\
    //socket init
    
    double init_val = 0;
    if(a == 0){
        bool init_flag = true;
        while(ros::ok()){
            send(sockfd, LJV_GetMeasurementValues, LJV_GetMeasurementValues_s, MSG_DONTROUTE);
            char recvline[1000];  //byte-buffer for response
            int n;
            usleep(5000); //simply wait x ms for controller to answer - replace this with an event-driven approach if needed
            n=read(sockfd,recvline,1000); //read socket/received data || ,0,NULL,NULL
            recvline[n]=0; //add string termination character 
            int i = 1;
            for(i =1; i<=16; i++){
                int msb, lsb, byte2, byte3, outStartBytePosition, outMeasurementValue, byteOffset; //integer-vairables
                double outMeasurementValueMM; //measurement value in mm-scale should be a float value
                byteOffset = (i-1) * 8; //every Out has an Offset of 8 byte to the prvious one (except Out1 which starts in byte 232) 
                outStartBytePosition = 232 + byteOffset; //Out1 measurement value starts with lsb at byte 232 therefore we add OutNumber*8 to 
                msb = (unsigned char)recvline[outStartBytePosition+3]; //due to network byte order, msb is the last of 4 bytes
                byte2 =  (unsigned char)recvline[outStartBytePosition+2]; //followed by byte #2
                byte3 = (unsigned char)recvline[outStartBytePosition+1];  //byte #3
                lsb = (unsigned char)recvline[outStartBytePosition]; //and lsb
                outMeasurementValue = msb << 24 | byte2 << 16 | byte3 << 8 | lsb; //shift bytes to big endian
                outMeasurementValueMM = outMeasurementValue * 0.00001; //since values are stored in 10nm units we have to multiply with factor 0.00001 to reach a mm-scale
                cloud[i-1].x = 0;
                cloud[i-1].y = 0;
                cloud[i-1].z = outMeasurementValueMM;                
                printf("%d :: Real OUT value = %fmm \n",i, outMeasurementValueMM); //output decimal value for OUT1-16
                mid_laser.data = outMeasurementValueMM;
                mid_laser_pub.publish(mid_laser);
            }
        //pcl::io::savePCDFileASCII("test_pcd.pcd", cloud);
        for(const auto& point: cloud)
            std::cerr <<"    "<<point.x<<"  "<<point.y<<"   "<<point.z << endl;  
        loop_rate.sleep();
        }
    }
    else{
        printf("not connecting \n ");
    }
   close(sockfd);
   return 0;
}