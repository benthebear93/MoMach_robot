#include "tcpip_basic.h"

void main()
{
	char s_x_buff[12] = { 0, }; //send x req
	char s_y_buff[12] = { 0, }; //send y req
	//char b_x_buff[11]; //bit read

	char r_x_buff[13] = { 0, }; // register read
	//char b_y_buff[11];
	char r_y_buff[13] = { 0, };
	float pos_x_y[2] = { 0.0, 0.0 };
	int ret = 0, ret2 = 0;
	float temp_x = 0, temp_y = 0, curr_x = 0, curr_y = 0;
	float y_encoder = 0, x_encoder = 0;
	float *p = 0;

	SOCKET sock; WSADATA wsa;
	SOCKET sock2; WSADATA wsa2;
	struct sockaddr_in sock_addr;
	struct sockaddr_in sock_addr2;

	WSAStartup(MAKEWORD(2, 2), &wsa);
	WSAStartup(MAKEWORD(2, 2), &wsa2);

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sock2 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&sock_addr, 0, sizeof(sock_addr));
	memset(&sock_addr2, 0, sizeof(sock_addr2));

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(DELTA_IP);
	sock_addr.sin_port = htons(DELTA_PORT);

	sock_addr2.sin_family = AF_INET;
	sock_addr2.sin_addr.s_addr = inet_addr(DELTA_IP);
	sock_addr2.sin_port = htons(DELTA_PORT);
	/////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	SOCKET hListen, hClient;
	SOCKADDR_IN tListenAddr = {};
	SOCKADDR_IN tClntAddr = {};

	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	tListenAddr.sin_family = AF_INET;
	tListenAddr.sin_port = htons(SERVER_PORT);
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr)) == SOCKET_ERROR);
	perror("Bind error");
	if(listen(hListen, SOMAXCONN) == SOCKET_ERROR);
	perror("Listen Error");

	int fromLen = sizeof(tClntAddr);
	hClient = accept(hListen, (SOCKADDR*)&tClntAddr, &fromLen);
	if (hClient == INVALID_SOCKET) {
		perror("Accept Error");
	}
	else {
		printf("%s connection complete\n", inet_ntoa(tClntAddr.sin_addr));
	}


	int first_check = connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
	int second_cehck = connect(sock2, (struct sockaddr*)&sock_addr2, sizeof(sock_addr2));
	printf("check 1: %d  check 2 : %d\n" , first_check , second_cehck);
	/*if (connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR || connect(sock2, (struct sockaddr*)&sock_addr2, sizeof(sock_addr2)) == SOCKET_ERROR) {
		printf("why here?\n");
		perror("connect()");
		closesocket(sock);
		closesocket(sock2);
		WSACleanup();
		return;
	}
	else {
		printf("connected\n");
	}*/
	memset(s_x_buff, 0, 12); //send buff set
	memset(s_y_buff, 0, 12); //send buff set
	memset(r_x_buff, 0, 11); //reg buff set
	memset(r_y_buff, 0, 11); //reg buff set

	while (1) {
		//printf("here?\n");
		char cBuffer[1024] = {};
		if (recv(hClient, cBuffer, 1024, 0) > 0) {
			printf("Recv Msg :%s\n", cBuffer);
		}
		const unsigned char x_value = 0x10;
		char cMsg[1];
		cMsg[0] = x_value;

		*s_x_buff = *set_s_buff(s_x_buff, x_add1, x_add2);
		*s_y_buff = *set_s_buff(s_y_buff, y_add1, y_add2);

		if (send(sock, s_x_buff, 12, 0) == SOCKET_ERROR) {
			perror("send()");
			closesocket(sock);
			WSACleanup();
			return;
		}
		if (send(sock2, s_y_buff, 12, 0) == SOCKET_ERROR) {
			perror("send2()");
			closesocket(sock2);
			WSACleanup();
			return;
		}
		ret = recv(sock, r_x_buff, 11, 0);
		ret2 = recv(sock2, r_y_buff, 11, 0);
		if (ret == SOCKET_ERROR) {
			perror("recv()"); closesocket(sock); WSACleanup();
			return;
		}
		if (ret2 == SOCKET_ERROR) {
			perror("recv()"); closesocket(sock2); WSACleanup();
			return;
		}
		char test[2];
		test[0] = (unsigned char)r_x_buff[9];
		test[1] = (unsigned char)r_x_buff[10];
		send(hClient, (char*)&test, sizeof(test), 0);
		PrintHexa(test, sizeof(test), reg_type);
		x_encoder = hex2decimal(r_x_buff);
		y_encoder = hex2decimal(r_y_buff);
		printf("x's current motor data : %f\n", x_encoder);
		
		p = Current_Pos(x_encoder, temp_x, y_encoder, temp_y);
		curr_x = curr_x - p[0];//increasing 
		curr_y = curr_y + p[1];//increasing 
	
		temp_x = x_encoder;
		temp_y = y_encoder;
		//printf("current x : %f :  ", curr_x);
		//printf(" current y : %f \n", curr_y);
	}
	closesocket(sock);
	closesocket(sock2);
	closesocket(hClient);
	closesocket(hListen);
	WSACleanup();
	system("pause");
}