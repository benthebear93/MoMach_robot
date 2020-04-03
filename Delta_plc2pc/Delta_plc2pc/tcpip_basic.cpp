#include <stdlib.h>
#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include "tcpip_basic.h"
#include <sstream>
using namespace std;

char * set_s_buff(char * s_buff, unsigned char add1, unsigned char add2 ){
	for (int i = 0; i < 5; i++) s_buff[i] = 0x00; //MBAP
	unsigned char len = 0x06; //length
	unsigned char unit_id = 0x01; //unit id
	unsigned char func = reg_read;//function code
	unsigned char read_len = add1; //0x35;//length
	unsigned char read_len2 = add2; //0x68; //length
	unsigned char data = 0x00; //read
	unsigned char data2 = 0x01; //len
	s_buff[5] = len;
	s_buff[6] = unit_id;
	s_buff[7] = func;
	s_buff[8] = read_len;
	s_buff[9] = read_len2;
	s_buff[10] = data;
	s_buff[11] = data2;
	return s_buff;
}

void PrintHexa(char* buff, size_t len, int type) { 
	size_t i; 
	if (type == bit_type) {
		printf("bit type receive : ");
		for (i = 0; i < len; i++){
			printf("%02X ", (unsigned char)buff[i]);
		}
		printf(" recive");
		printf("\n");
	}
	else if(type == reg_type){
		printf("reg type receive : ");
		for (i = 0; i < len; i++){
			printf("%02X ", (unsigned char)buff[i]);
		}
		printf(" recive");
		printf("\n");
	}
	else {
		printf("send             : ");
		for (i = 0; i < len; i++){
			printf("%02X ", (unsigned char)buff[i]);
		}
		printf(" send");
		printf("\n");
	}
}

int check_data(char *buff) {
	int num1 = 0, num2 = 0, num3 = 0;
	//printf("%02X      %02X \n", (unsigned char)buff[9], (unsigned char)buff[10]);
	num1 = (unsigned char)buff[9];
	num2 = (unsigned char)buff[10];
	num3 = (num1 << 8) | (num2);
	return num3;
}

float *Current_Pos(float encoder_x, float temp_x, float encoder_y, float temp_y) {
	float increase_x = 0;
	float increase_y = 0;
	if (temp_x > encoder_x || temp_y > encoder_y) {
		if (temp_x > 60000|| temp_y > 60000) { // increase case 60000 to 100
			//printf(" encoder_x : %f  temp_x : %f ", encoder_x, temp_x);
			increase_x = ((65535 - temp_x) + encoder_x) / (266.2 * 1000);
			increase_y = ((65535 - temp_y) + encoder_y) / (65.5 * 1000);
		}
		else { // decrease
			increase_x = (encoder_x - temp_x) / (266.2 * 1000);
			increase_y = (encoder_y - temp_y) / (65.5 * 1000);
		}
	}
	else {
		increase_x = (encoder_x - temp_x) / (266.2 * 1000);
		increase_y = (encoder_y - temp_y) / (65.5 * 1000);
	}
	//increase_x = (encoder_x - temp_x) / (266.2 * 1000);
	//increase_y = (encoder_y - temp_y) / (65.5 * 1000);
	//printf("increase_x : %f ", increase_x);
	//printf("increase_y : %f ", increase_y);
	static float curr_x_y[2];
	curr_x_y[0] = increase_x;
	curr_x_y[1] = increase_y;
	return curr_x_y;
}
