#include "tcpip_basic.h"

void main()
{
	FILE *out;
	out = fopen("test.txt", "w");
	char s_x_buff[12]; //send x req
	char s_y_buff[12]; //send y req
	//char b_x_buff[11]; //bit read
	char r_x_buff[13]; // register read
	//char b_y_buff[11];
	char r_y_buff[13];
	float pos_x_y[2] = { 0.0, 0.0 };
	int ret = 0;
	int ret2 = 0;
	float temp_x = 0;
	float temp_y = 0;
	float curr_x = 0, curr_y = 0;
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

	int first_check = connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
	int second_cehck = connect(sock2, (struct sockaddr*)&sock_addr2, sizeof(sock_addr2));
	printf("check 1: %d  check 2 : %d\n" , first_check , second_cehck);
	/*if (connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR || connect(sock2, (struct sockaddr*)&sock_addr2, sizeof(sock_addr2)) == SOCKET_ERROR) {
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
	int a = 0;
	while (1) {
		a = a + 1;
		*s_x_buff = *set_s_buff(s_x_buff, x_add1, x_add2);
		*s_y_buff = *set_s_buff(s_y_buff, y_add1, y_add2);
		//printf("*s_x_buff %d", *s_x_buff);

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
		x_encoder = check_data(r_x_buff);
		//printf("%d x's current motor data : %f\n",a, x_encoder);

		y_encoder = check_data(r_y_buff);
		//printf("%d y's current motor data : %f\n", a, y_encoder);
		
		p = Current_Pos(x_encoder, temp_x, y_encoder, temp_y);
		curr_x = curr_x + p[0];//increasing 
		curr_y = curr_y + p[1];//increasing 
		//printf("%f     \n", p[0]);
		/*if (x_encoder - temp_x > 0) {
			printf("plus : x_encoder - temp_x : %f", x_encoder - temp_x);
			curr_x = curr_x + p[0];//increasing 
			printf("current x : %f :  \n", curr_x);
		}
		else {
			printf("x_encoder - temp_x : %f\n", x_encoder - temp_x);
			curr_x = curr_x + p[0];
			printf("current x : %f \n:  ", curr_x);
		}
		if (y_encoder - temp_y > 0) curr_y = curr_y + p[1];//increasing 
		else curr_y = curr_y - p[1];
		*/
		temp_x = x_encoder;
		temp_y = y_encoder;
		printf("current x : %f :  ", curr_x);
		printf(" current y : %f \n", curr_y);
		//if (temp_x == 0 && temp_y == 0) printf("something wrong all the data is 0 \n");
		//fprintf(out, "%d : x encoder :  %d\n", a,  x_encoder);
	}
	fclose(out);
	closesocket(sock);
	closesocket(sock2);
	WSACleanup();
	system("pause");
}