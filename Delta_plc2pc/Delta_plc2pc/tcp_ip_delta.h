#include <stdlib.h>
#include <stdio.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include "tcp_ip_delta.h"
using namespace std;
#pragma comment(lib, "WS2_32.LIB")
#pragma once
#define DELTA_IP "192.168.0.158"
#define DELTA_PORT 502
#define M_type 800
#define reg_type 1
#define bit_type 2

const unsigned char b_read = 0x01;
const unsigned char b_write = 0x05;
const unsigned char reg_read = 0x03;
void PrintHexa(char* buff, size_t len, int type);