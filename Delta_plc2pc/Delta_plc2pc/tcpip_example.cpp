#include <stdlib.h>
#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "WS2_32.LIB")
//#define DELTA_IP "192.168.100.158"
#define DELTA_IP "192.168.0.158"
#define DELTA_PORT 502

void PrintHexa(char* buff, size_t len);

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
	int c_num = 0;
	while(1) {
		memset(s_buff, 0, 12);
		memset(b_buff, 0, 10);
		memset(r_buff, 0, 13);
		// build form 00 00 / 00 00 /00 06/FF/FF/ FF FF FF FF  = 24 
		//8C4
		{
			for (int i = 0; i < 5; i++) s_buff[i] = 0x00; //MBAP
			unsigned char len = 0x06; //length
			unsigned char unit_id = 0x01; //unit id
			unsigned char func = 0x01; //func(bit read discret : 01, coil : 02,  read input register : 04, read holding  reg : 03)
			unsigned char read_len = 0x08;//length
			unsigned char read_len2 = 0x0A; //length
			unsigned char data = 0x00; //read
			unsigned char data2 = 0x10; //len
			s_buff[5] = len;
			s_buff[6] = unit_id;
			s_buff[7] = func;
			s_buff[8] = read_len;
			s_buff[9] = read_len2;
			s_buff[10] = data;
			s_buff[11] = data2;
		}
		PrintHexa(s_buff, 12);
		send(sock, s_buff, 12, 0);
		if (send(sock, s_buff, 12, 0) == SOCKET_ERROR) {
			perror("send()");
			closesocket(sock);
			WSACleanup();
			return;
		}
		c_num = c_num + 1;
		printf("%d  recieve : ", c_num);
		memset(b_buff, 0, 10);
		memset(r_buff, 0, 13);
		ret = recv(sock, b_buff, 10, 0);
		ret = recv(sock, r_buff, 13, 0);
		if (ret == SOCKET_ERROR) {
			perror("recv()"); closesocket(sock); WSACleanup();
			return;
		}
		PrintHexa(b_buff, ret);
		printf("ret(data byte) =  %d\n", ret);
	}
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