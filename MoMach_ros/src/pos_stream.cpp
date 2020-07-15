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
   MoMach_ros::pos_stream pos;
   ros::init(argc, argv, "pos_stream");
   ros::NodeHandle nh;
   ros::Publisher laser_pub = nh.advertise<MoMach_ros::pos_stream>("pos_stream", 100);
   ros::Rate loop_rate(1000); 
   char s_buff[12]; //send
   char b_buff[11]; //bit read
   char r_buff[13]; // register read

   struct sockaddr_in sock_addr;
   int ret = 0;
   int sockfd;
   sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   memset(&sock_addr, 0, sizeof(sock_addr));

   sock_addr.sin_family = AF_INET;
   sock_addr.sin_addr.s_addr = inet_addr(DELTA_IP);
   sock_addr.sin_port = htons(DELTA_PORT);

	if (connect(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1) {
		perror("connect()");
		close(sockfd);
		return 0;
	}
	while(ros::ok()){
		memset(s_buff, 0, 12);
		memset(b_buff, 0, 10);
		memset(r_buff, 0, 13);
	// build form 00 00 / 00 00 /00 06/FF/FF/ FF FF FF FF  = 24 
		//8C4
		{
			for (int i = 0; i < 5; i++) s_buff[i] = 0x00; //MBAP
			unsigned char len = 0x06; //length
			unsigned char unit_id = 0x01; //unit id
			unsigned char func = reg_read; //func(bit read discret : 01, coil : 02,  read input register : 04, read holding  reg : 03)
			unsigned char read_len = 0x53;//length
			unsigned char read_len2 = 0x104; //length
			unsigned char data = 0x00; //read
			unsigned char data2 = 0x01; //len
			s_buff[5] = len;
			s_buff[6] = unit_id;
			s_buff[7] = func;
			s_buff[8] = read_len;
			s_buff[9] = read_len2;
			s_buff[10] = data;
			s_buff[11] = data2;
		}
		//PrintHexa(s_buff, 12);
		//send(sock, s_buff, 12, 0);
		if (send(sockfd, s_buff, 12, 0) == -1) {
			perror("send()");
			close(sockfd);
			return 0;
		}
		//memset(b_buff, 0, 10);
		memset(r_buff, 0, 11);
		ret = recv(sockfd, r_buff, 11, 0);
		if (ret == -1) {
			perror("recv()"); 
			close(sockfd);
			return 0;
		}
		printf("recv : ");
		PrintHexa(r_buff, ret);
		loop_rate.sleep();
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