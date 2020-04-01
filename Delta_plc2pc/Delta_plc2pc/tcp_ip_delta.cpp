#include <stdlib.h>
#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include "tcp_ip_delta.h"
using namespace std;
#pragma comment(lib, "WS2_32.LIB")
//#define DELTA_IP "192.168.100.158"
#define DELTA_IP "192.168.0.158"
#define DELTA_PORT 502
#define M_type 800
#define reg_type 1
#define bit_type 2
const unsigned char b_read = 0x01;
const unsigned char b_write = 0x05;
const unsigned char reg_read = 0x03;
void PrintHexa(char* buff, size_t len, int type);

void main()
{
	SOCKET sock;WSADATA wsa;
	char s_buff[12]; //send
	char b_buff[11]; //bit read
	char r_buff[13]; // register read
	struct sockaddr_in sock_addr;
	int ret = 0;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(DELTA_IP);
	sock_addr.sin_port = htons(DELTA_PORT);

	if (connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
		perror("connect()");
		closesocket(sock);
		WSACleanup();
		return;
	}
	else {
		printf("connected\n");
	}
	int c_num = 0;
	while(1) {
		memset(s_buff, 0, 12);
		memset(b_buff, 0, 11);
		memset(r_buff, 0, 13);
		// build form 00 00 / 00 00 /00 06/FF/FF/ FF FF FF FF  = 24 
		{
			for (int i = 0; i < 5; i++) s_buff[i] = 0x00; //MBAP
			unsigned char len = 0x06; //length
			unsigned char unit_id = 0x01; //unit id
			unsigned char func = reg_read;//function code
			unsigned char read_len = 0x35;//length
			unsigned char read_len2 = 0x68; //length
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
		c_num = c_num + 1;
		//printf("%d send : ", c_num);
		PrintHexa(s_buff, 12, 0);
		send(sock, s_buff, 12, 0);
		if (send(sock, s_buff, 12, 0) == SOCKET_ERROR) {
			perror("send()");
			closesocket(sock);
			WSACleanup();
			return;
		}
		//printf("%d recieve : ", c_num);

		memset(b_buff, 0, 11);
		memset(r_buff, 0, 11);
		if (s_buff[7] == b_read) {
			ret = recv(sock, b_buff, 11, 0);
			printf("length : %d\n", ret);
		}
		else {
			ret = recv(sock, r_buff, 11, 0);
			printf("length : %d \n", ret);
			
		}
		if (ret == SOCKET_ERROR) {
			perror("recv()"); closesocket(sock); WSACleanup();
			return;
		}
		if (s_buff[7] == b_read) {
			PrintHexa(b_buff, ret, bit_type);
		}
		else {
			PrintHexa(r_buff, ret, reg_type);
		}
	}
	closesocket(sock); 
	WSACleanup(); 
	system("pause");
}
void PrintHexa(char* buff, size_t len, int type) { 
	size_t i; 
	if (type == bit_type) {
		printf("bit type receive : ");
		for (i = 0; i < len; i++)
		{
			printf("%02X ", (unsigned char)buff[i]);
		}
		printf(" recive");
		printf("\n");
	}
	else if(type == reg_type){
		printf("reg type receive : ");
		for (i = 0; i < len; i++)
		{
			printf("%02X ", (unsigned char)buff[i]);
		}
		printf(" recive");
		printf("\n");
	}
	else {
		printf("send             : ");
		for (i = 0; i < len; i++)
		{
			printf("%02X ", (unsigned char)buff[i]);
		}
		printf(" send");
		printf("\n");
	}
}