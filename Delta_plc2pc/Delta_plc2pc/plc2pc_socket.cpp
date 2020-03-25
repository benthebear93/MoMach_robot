#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <string.h>
#pragma comment (lib , "ws2_32.lib")

#define PORT 6000
#define PACKET_SIZE 16
#define SERVER_IP "192.168.0.158"
#define MAXLINE 1024

void showError(const char * msg)
{
	std::cout << "error : " << msg << std::endl;
	exit(-1);
}

void main()
{
	WSADATA data;
	::WSAStartup(MAKEWORD(2, 2), &data);
	struct sockaddr_in sock_addr;
	SOCKET client;
	client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&sock_addr, 0, sizeof(sock_addr));

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	sock_addr.sin_port = htons(PORT);
	{
		char local_name[1024];
		struct hostent *host_ptr = NULL;
		memset(local_name, 0, 1024);
		host_ptr = gethostbyname(local_name);
		printf("Clinet IP : %u.%u.%u.%u\n",
			(unsigned char)host_ptr->h_addr_list[0][0],
			(unsigned char)host_ptr->h_addr_list[0][1],
			(unsigned char)host_ptr->h_addr_list[0][2],
			(unsigned char)host_ptr->h_addr_list[0][3]);
	}
	printf("here?\n");
	if (client == INVALID_SOCKET)
		showError("clinet socket fail");

	/*if (connect(client, (sockaddr *)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
		showError("connection fail\n");
	}
	else {
		connect(client, (sockaddr *)&sock_addr, sizeof(sock_addr));
		printf("connected\n");
	}*/
	connect(client, (sockaddr *)&sock_addr, sizeof(sock_addr));
	{
		char io_buff[MAXLINE];
		char comm_buff[MAXLINE];
		memset(io_buff, 0, MAXLINE);
		scanf("%s", io_buff);
		printf(comm_buff, "[client]: %s", io_buff);
		send(client, comm_buff, 1024, 0);
		printf("%s\n", comm_buff);
	}


	/*char cBuffer[MAXLINE];
	memset(cBuffer, 0x00, MAXLINE);
	printf("here\n");
	while (1) {
		printf("while running? \n");
		if (recv(client, cBuffer, PACKET_SIZE, 0) != -1) {
			printf("returned\n");
			printf("RECV Msg : %s\n", cBuffer);
		}
		else {
			printf("Not returned");
			break;
		}
	}*/
	closesocket(client);
	::WSACleanup();
	system("pause");
}
