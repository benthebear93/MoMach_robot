import socket
import time
import struct
import time
import sys
#endianness = [
#    ('@', 'native, native'),
#    ('=', 'native, standard'),
#    ('<', 'little-endian'),
#    ('>', 'big-endian'),
#    ('!', 'network'),
#    ]

HOST = '192.168.0.158'
PORT = 502

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))

curr_x = 0
curr_y = 0
temp_x = 0
temp_y = 0
dx =0
dy =0

def set_modbus_request(unit, cmd, addr, addr2):
    if addr > 10000:
        addr = addr % 10000 - 1
    packet = struct.pack(">BBBBBBBBBBBB", 0, 0, 0, 0, 0, 6, unit, cmd, addr, addr2, 0, 1)
    return packet
x_ch = []
y_ch = []
while True:
    req_x = set_modbus_request(1,3,53,104)
    req_x2 = set_modbus_request(1,3,53,105)
    req_y = set_modbus_request(1,3,53,106)
    req_y2 = set_modbus_request(1,3,53,107)
    
    client_socket.sendall(req_x)
    x_data = client_socket.recv(13)
    client_socket.sendall(req_x2)
    x2_data = client_socket.recv(13)
    client_socket.sendall(req_y)
    y_data = client_socket.recv(13)
    client_socket.sendall(req_y2)
    y2_data = client_socket.recv(13)
    #print("".join("\\x{:02x}".format(c) for c in x_data))
    #print("".join("\\x{:02x}".format(c) for c in x2_data))
    
    monitor_x = struct.unpack('>f', (x2_data[9:11]+x_data[9:11]))[0]
    monitor_y = struct.unpack('>f', (y2_data[9:11]+y_data[9:11]))[0]
    print("##############################")
    print("                              ")
    print("curr x  :", monitor_x)
    print("curr y  :", monitor_y)
    print("                              ")
    print("##############################")
client_socket.close()

