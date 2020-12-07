import math
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import pandas as pd
import csv
import pptk

f = open('C:/Users/UNIST/Dropbox/MS_course/Mobile_machining/machining/disk_data/runout_error/m_record-sixteen_msg.csv', 'r')
data = csv.reader(f)
print(data)
lines = []
x = []
y = []
z = []
check = 0
x1 = []
y1 = []
z1 = []
xyz_2 = []
temp = 0
add_line = 0
yy = 0
for line in data:
    check += 1
    #print("running? a :", check)
    # if check > 17000:
    #     break
    #print("line[0]:", line[0])
    end_pos = len(line[0])-1
    #print(end_pos)s
    strings = line[0][1:end_pos].split(',')
    if temp == -26.4 and round(float(strings[a+1]), 4) > -1.5:
        add_line += 0.16
        #print(check)
        check = 0
    for a in range(0, 48, 3):
        xyz = []
        x_round = round(float(strings[a])*10 + add_line, 4)
        #print(x_round)
        y_round = round(float(strings[a+1]), 4)
        yy = y_round
        temp = round(y_round, 1)
        #print(temp)
        z_round = round(float(strings[a+2]), 4)
        #print("ori : ", float(strings[a]), " ", float(strings[a+1]), " ", float(strings[a+2])*1000)
        #x.append(x_round)
        #y.append(y_round)
        #z.append(z_round)
        xyz.append(x_round)
        xyz.append(y_round)
        xyz.append(z_round)
        xyz_2.append(xyz)
        #print(x_round, y_round, z_round)
    # x1.append(x)
    # y1.append(y)
    # z1.append(z)
f.close()
print("csv closed, start plotting")
xyz_2 = np.array(xyz_2)
# x = np.array(x1)
# print("numpy x done")

# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')
# y = np.array(y1)
# print("numpy y done")
# z = np.array(z1)
# print("numpy z done")
print("numpy changed")
v = pptk.viewer(xyz_2)
v.set(point_size = 0.0001)
# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')
# ax.plot_wireframe(x, y, z, rstride=1, cstride=1, lw=1, color="red")
# print("wire frame setted")
# ax.set_xlabel('x axis')
# ax.set_ylabel('y axis')
# ax.set_zlabel('z axis')
# plt.show()

