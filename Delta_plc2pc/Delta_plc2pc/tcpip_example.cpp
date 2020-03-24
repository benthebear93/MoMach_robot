#include <stdlib.h>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "WS2_32.LIB")
//#define NE_IP "192.168.100.158"
#define NE_IP "192.168.0.158"
#define NE_PORT 6000

void PrintHexa(char* buff, size_t len);

void main()
{
	SOCKET sock;
	WSADATA wsa;
	char s_buff[8];
	char r_buff[7];
	struct sockaddr_in sock_addr;
	int ret;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(NE_IP);
	sock_addr.sin_port = htons(NE_PORT);

	if (connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
		perror("connect()");
		closesocket(sock);
		WSACleanup();
		return;
	}

	memset(s_buff, 0, 7);
	memset(r_buff, 0, 7);
	{ //char = 1byte 0~255
		unsigned char d_id = 0x06;
		unsigned char d_rw = 0x01;
		unsigned char d_way = 03;
		unsigned char d_add_base = 0x32;
		unsigned char d_add = 0x58;
		unsigned char read = 00;
		unsigned char read_r = 01;
		s_buff[0] = d_id;
		s_buff[1] = d_rw;
		s_buff[2] = d_way;
		s_buff[3] = d_add_base;
		s_buff[4] = d_add;
		s_buff[5] = read;
		s_buff[6] = read_r;
	}
	PrintHexa(s_buff, 7);
	send(sock, s_buff, 7, 0);
	if (send(sock, s_buff, 7, 0) == SOCKET_ERROR) {
		perror("send()");
		closesocket(sock);
		WSACleanup();
		return;
	}
	printf("waiting data to full at buff (MSG_WAITALL )");
	ret = recv(sock, r_buff, 7, MSG_WAITALL);
	printf("ret(data byte) =  %d\n", ret);
	if (ret == SOCKET_ERROR) { 
		perror("recv()"); closesocket(sock); WSACleanup(); 
		return; 
	} 
	PrintHexa(r_buff, ret);
	//printf("ID : %02X\n", (unsigned char)r_buff[4]); 
	closesocket(sock); 
	WSACleanup(); 
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