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
	SOCKET sock;
	WSADATA wsa;
	char s_buff[12];
	char r_buff[12];
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
	memset(s_buff, 0, 12);
	memset(r_buff, 0, 12);
	// build form 00 00 / 00 00 /00 06/FF/FF/ FF FF FF FF  = 24 
	//8C4
	{ 
		
		//for (int i = 0; i < 5; i++) s_buff[i] = 0x00; //MBAP
		unsigned char d_id = 0x06; //length
		unsigned char d_rw = 0x01; //unit
		unsigned char d_way = 0x03; //func
		unsigned char d_add_base = 0x04;//length
		unsigned char d_add = 0x10; //length
		unsigned char read = 0x00; //read
		unsigned char read_r = 0x01; //len
		s_buff[0] = 0x00;
		s_buff[1] = 0xC0;
		s_buff[2] = 0x00;
		s_buff[3] = 0x00;
		s_buff[4] = 0x00;
		s_buff[5] = d_id;
		s_buff[6] = d_rw;
		s_buff[7] = d_way;
		s_buff[8] = d_add_base;
		s_buff[9] = d_add;
		s_buff[10] = read;
		s_buff[11] = read_r;
	}
	PrintHexa(s_buff, 12);
	send(sock, s_buff, 12, 0);
	if (send(sock, s_buff, 12, 0) == SOCKET_ERROR) {
		perror("send()");
		closesocket(sock);
		WSACleanup();
		return;
	}
	/*while (ret == 0) {
		ret = recv(sock, r_buff, 12, 0);
		printf("ret(data byte) =  %d\n", ret);
	}*/
	ret = recv(sock, r_buff, 12, 0);
	PrintHexa(r_buff, 12);
	printf("ret(data byte) =  %d\n", ret);
	if (ret == SOCKET_ERROR) { 
		perror("recv()"); closesocket(sock); WSACleanup(); 
		return; 
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