#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "MoMach_ros/pos_stream.h"
#include <typeinfo>
#include <time.h>
#include <math.h>

using namespace std;
#define DELTA_IP "192.168.0.158"
#define DELTA_PORT 502
#define reg_type 1
#define bit_type 2

const unsigned char b_read = 0x01;
const unsigned char b_write = 0x05;
const unsigned char reg_read = 0x03;

void PrintHexa(char* buff, size_t len);

int main(int argc, char**argv){
	time_t start, end;
	int curr_time = 0;

	MoMach_ros::pos_stream pos;
	std_msgs::Float32 pos_y;
	ros::init(argc, argv, "pos_stream");
	ros::NodeHandle nh;
	ros::Publisher pos_pub = nh.advertise<MoMach_ros::pos_stream>("pos_stream", 100);
	ros::Publisher posy = nh.advertise<std_msgs::Float32>("posy", 100);
	ros::Rate loop_rate(500); 
	char s_buff[12]; //send
	char s2_buff[12]; //send
	char x_r_buff[13]; // register read
	char y_r_buff[13]; // register read
	char z_r_buff[13]; // register read

	struct sockaddr_in sock_addr;
	int ret = 0, ret2 = 0, ret3 = 0, ret4 = 0;
	int sockfd;
	sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	sock_addr.sin_family = PF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(DELTA_IP);
	sock_addr.sin_port = htons(DELTA_PORT);
	char x_s_buff[]={0x00, 0x00, 0x00, 0x00, 0x00,\
		0x06, 0x01, 0x03, 0x35, 0x68, \
		0x00, 0x02};
	char y_s_buff[]={0x00, 0x00, 0x00, 0x00, 0x00,\
		0x06, 0x01, 0x03, 0x35, 0x70, \
		0x00, 0x02};
	char z_s_buff[]={0x00, 0x00, 0x00, 0x00, 0x00,\
		0x06, 0x01, 0x03, 0x35, 0x6C, \
		0x00, 0x02};
	if (connect(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
		perror("connect()");
		close(sockfd);
		return 0;
	} // connect check
	//int data = 0;
	while(ros::ok()){
		send(sockfd, x_s_buff, 12, 0);
		ret = recv(sockfd, x_r_buff, 13, 0);
		send(sockfd, y_s_buff, 12, 0);
		ret2 = recv(sockfd, y_r_buff, 13, 0);
		send(sockfd, z_s_buff, 12, 0);
		ret3 = recv(sockfd, z_r_buff, 13, 0);
		
		char x_data[] = {x_r_buff[10], x_r_buff[9], x_r_buff[12], x_r_buff[11]};
		char y_data[] = {y_r_buff[10], y_r_buff[9], y_r_buff[12], y_r_buff[11]};
		char z_data[] = {z_r_buff[10], z_r_buff[9], z_r_buff[12], z_r_buff[11]};
		float output_x = *(float*)&x_data;
		float output_y = *(float*)&y_data;
		float output_z = *(float*)&z_data;
		// int temp = output_z*1000;
		// output_z = temp/1000.0;
		printf("x = %f  | y = %f | z = %f\n", output_x, output_y, output_z);
		pos_y.data = output_y;
		pos.pos_x = output_x*1000;
		pos.pos_y = output_y*1000;
		pos.pos_z = output_z*1000;
		pos_pub.publish(pos);
		posy.publish(pos_y);
		loop_rate.sleep();
	}
	close(sockfd); 
}

void PrintHexa(char* buff, size_t len) { 
	size_t i; 
	for (i = 0; i < len; i++) 
	{ 
		printf("%02X ", (unsigned char)buff[i]); 
	} 
	printf("\n"); 
}