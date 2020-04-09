import socket
import time
import binascii
import struct
import sys
import select
import time
#endianness = [
#    ('@', 'native, native'),
#    ('=', 'native, standard'),
#    ('<', 'little-endian'),
#    ('>', 'big-endian'),
#    ('!', 'network'),
#    ]

HOST = '192.168.0.158'
PORT = 502

def Increment(x_encoder, y_encoder, temp_x, temp_y):

    dx = 0
    dy = 0;
    if temp_x > x_encoder:
        if temp_x - x_encoder > 35000:
            dx = (65535 - temp_x + x_encoder) / (226.5 * 1000)
        else:
            dx = -1* (temp_x - x_encoder) / (226.5  * 1000)
    elif temp_x < x_encoder:
        if x_encoder - temp_x > 35000:
            dx = (65535 - x_encoder + temp_x) / (226.5  * 1000)
        else:
            dx = (x_encoder - temp_x) / (226.5 * 1000)
    else:
        dx = 0
        
    if temp_y > y_encoder:
        if temp_y - y_encoder > 35000:
            dy = (65535 - temp_y + y_encoder) / (66.5  * 1000)
        else:
            dy = -1 * (temp_y - y_encoder) / (66.5  * 1000)
    elif temp_y < y_encoder:
        if y_encoder - temp_y > 35000:
            dy = (65535 - y_encoder + temp_y) / (66.5 * 1000)
        else:
            dy = (y_encoder - temp_y) / (66.5  * 1000)
    else:
        dy = 0
    return dx, dy

print("connect here?")
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#client_socket2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))
#client_socket2.connect((HOST, PORT))
curr_x = 0
curr_y = 0
temp_x = 0
temp_y = 0


def set_modbus_request(unit, cmd, addr, addr2):
    if addr > 10000:
        addr = addr % 10000 - 1
    # transaction identifier (2), protocol identifier (2), length (2), 
    packet = struct.pack(">BBBBBBBBBBBB", 0, 0, 0, 0, 0, 6, unit, cmd, addr, addr2, 0, 1)
    return packet

a  =0
while True:
    #msg = '\00\00\00\00\00\06\01\03\35\68\00\01' #string
    #data = msg.encode() #byte type object
    #length = len(data)
    print("here?")
    req_x = set_modbus_request(1,3,53,104)
    req_y = set_modbus_request(1,3,53,106)
    #print("".join("\\x{:02x}".format(c) for c in data))
    x_ch = [0]
    y_ch = [0]
    client_socket.sendall(req_x);
    x_data = client_socket.recv(13)
    client_socket.sendall(req_y);
    y_data = client_socket.recv(13)
        
    x_encoder = x_data[9] << 8
    x_encoder += x_data[10]
    
    y_encoder = y_data[9] << 8
    y_encoder += y_data[10]
  
    
    dx, dy = Increment(x_encoder, y_encoder, temp_x, temp_y)
    curr_x += dx
    curr_y += dy
      
    temp_x = x_encoder
    temp_y = y_encoder
    a =a+1
    print(a,"x_val :", curr_x)
    print("y_val :", curr_y)
    
client_socket.close()


