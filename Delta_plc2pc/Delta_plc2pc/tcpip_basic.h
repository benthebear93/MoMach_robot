#include <stdlib.h>
#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <math.h>
#include <cstring>
using namespace std;
#pragma comment(lib, "WS2_32.LIB")
#pragma once
#define DELTA_IP "192.168.0.158"
#define RPI_IP "192.168.0.4"
#define DELTA_PORT 502
#define M_type 800
#define reg_type 1
#define bit_type 2
#define SOMAXCONN       0x7fffffff 
#define SERVER_PORT 3000

const unsigned char b_read = 0x01;
const unsigned char b_write = 0x05;
const unsigned char reg_read = 0x03;

const unsigned char x_add1 = 0x35;
const unsigned char x_add2 = 0x68;
const unsigned char y_add1 = 0x35;
const unsigned char y_add2 = 0x6A;

void PrintHexa(char* buff, size_t len, int type);
int hex2decimal(char *buff);
char *set_s_buff(char * s_buff, unsigned char add1, unsigned char add2);
float *Current_Pos(float encoder_x, float temp_x, float encoder_y, float temp_y);