//#include "ros/ros.h"
//#include "std_msgs/Float32.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include "MoMach_ros/pos_stream.h"
#include <typeinfo>
#include <time.h>

using namespace std;
//#define DELTA_IP "192.168.100.158"
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

	//MoMach_ros::pos_stream pos;
	//ros::init(argc, argv, "pos_stream");
	//ros::NodeHandle nh;
	//ros::Publisher laser_pub = nh.advertise<MoMach_ros::pos_stream>("pos_stream", 100);
	//ros::Rate loop_rate(100); 
	//char s_buff[12]; //send
	//char s2_buff[12]; //send
	char x_r_buff[11]; // register read
	char x_r2_buff[11]; // register read
	char y_r_buff[11]; // register read
	char y_r2_buff[11]; // register read

	struct sockaddr_in sock_addr;
	int ret = 0, ret2 = 0, ret3 = 0, ret4 = 0;
	int sockfd;
	sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&sock_addr, 0, sizeof(sock_addr));

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(DELTA_IP);
	sock_addr.sin_port = htons(DELTA_PORT);
	char x_s_buff[]={0x00, 0x00, 0x00, 0x00, 0x00,\
		0x06, 0x01, 0x03, 0x35, 0x68, \
		0x00, 0x01};
	char x_s2_buff[]={0x00, 0x00, 0x00, 0x00, 0x00,\
		0x06, 0x01, 0x03, 0x35, 0x69, \
		0x00, 0x01};
	char y_s_buff[]={0x00, 0x00, 0x00, 0x00, 0x00,\
		0x06, 0x01, 0x03, 0x35, 0x70, \
		0x00, 0x01};
	char y_s2_buff[]={0x00, 0x00, 0x00, 0x00, 0x00,\
		0x06, 0x01, 0x03, 0x35, 0x71, \
		0x00, 0x01};

	if (connect(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
		perror("connect()");
		close(sockfd);
		return 0;
	} // connect check
	start = time(NULL);
	int DataNum = 0;
	//ros::ok()
	int a = 0;
	while(){
		DataNum +=1;
		//memset(x_s_buff, 0, 12);
		//memset(r_buff, 0, 11); //reset buffers
		send(sockfd, x_s_buff, 12, 0);
		ret = recv(sockfd, x_r_buff, 11, 0);
		send(sockfd, x_s2_buff, 12, 0);
		ret2 = recv(sockfd, x_r2_buff, 11, 0); //recv

		send(sockfd, y_s_buff, 12, 0);
		ret3 = recv(sockfd, y_r_buff, 11, 0);
		send(sockfd, y_s2_buff, 12, 0);
		ret4 = recv(sockfd, y_r2_buff, 11, 0);

		char x_data[] = {x_r_buff[10], x_r_buff[9], x_r2_buff[10], x_r2_buff[9]};
		char y_data[] = {y_r_buff[10], y_r_buff[9], y_r2_buff[10], y_r2_buff[9]};
		float output_x = *(float*)&x_data;
		float output_y = *(float*)&y_data;

		//printf("x data =  %f\n", output_x);
		//printf("y data =  %f\n", output_y);
		end = time(NULL);
		double passed_time =(double)(end - start);
		//printf("passed_time : %f\n", passed_time);
		if(passed_time >= 100){
			printf("%d ", DataNum);
			break;
		}
		//loop_rate.sleep();
	}
	close(sockfd); 
	system("pause");
}

void PrintHexa(char* buff, size_t len) { 
	size_t i; 
	for (i = 0; i < len; i++) 
	{ 
		printf("%02X ", (unsigned char)buff[i]); 
	} 
	printf("\n"); 
}