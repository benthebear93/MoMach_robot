#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib , "ws2_32.lib")

#define PORT 6000
#define PACKET_SIZE 16
#define SERVER_IP "192.168.0.158"
void showError(const char * msg)
{
	std::cout << "error : " << msg << std::endl;
	exit(-1);
}

int main()
{
	WSADATA data;
	::WSAStartup(MAKEWORD(2, 2), &data);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (client == INVALID_SOCKET)
		showError("clinet socket fail");

	sockaddr_in addr = { 0 };

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("192.168.0.158");
	addr.sin_port = htons(6000);

	if (connect(client, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
		showError("connection fail\n");
	}
	else {
		connect(client, (sockaddr *)&addr, sizeof(addr));
		printf("connected\n");
	}
	/*int bufSize = 1024 * 4;
	socklen_t len = sizeof(bufSize);
	setsockopt(client, SOL_SOCKET, SO_SNDBUF, &bufSize, sizeof(bufSize));
	getsockopt(client, SOL_SOCKET, SO_SNDBUF, &bufSize, &len);
	printf("socket buffer size: %d\n", bufSize);*/

	char cBuffer[1] = {};
	int check = recv(client, cBuffer, 1, 0);
	printf("check :%d", check);
	if (recv(client, cBuffer, PACKET_SIZE, 0) != -1) {
		printf("returned\n");
		printf("RECV Msg : %s\n", cBuffer);
	}
	else {
		printf("Not returned");
	}
	closesocket(client);
	::WSACleanup();
	return 0;
}
