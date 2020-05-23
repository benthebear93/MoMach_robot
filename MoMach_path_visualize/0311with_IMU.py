
from matplotlib import pyplot as plt
from matplotlib import animation
import numpy as np
import keyboard
import time
# import nidaqmx
import pyrealsense2 as rs
import time
from pprint import pprint


# x_start = 0
# y_start = 0
# x_final = 0
# y_final = 0

xlog = [];
ylog = [];

x_current = 0
y_current = 0
#
# tic_x_start = False
# tic_y_start = False
# tic_x_final = False
# tic_y_final = False
# tic_key = False
sketch_key = False

cfg = rs.config()

pipe = rs.pipeline()
cfg.enable_stream(rs.stream.pose)  # Positional data (translation, rotation, velocity etc)
#cfg.enable_stream(rs.stream.fisheye, 1)  # Left camera
#cfg.enable_stream(rs.stream.fisheye, 2)  # Right camera
pipe.start(cfg)

while True:#making a loop
    # if keyboard.is_pressed('q'):
    #     pipe.stop()
    #     break
    # with nidaqmx.Task() as task:
    #     task.ai_channels.add_ai_current_chan("cDAQ1Mod2/ai0")
    #     f = task.read() * 100000 - 400
    #     f = round(f, 1)
    # while (tic_key == False):
    #
    #     with nidaqmx.Task() as task:
    #         task.ai_channels.add_ai_current_chan("cDAQ1Mod2/ai0")
    #         f = task.read() * 100000 - 400
    #         f = round(f, 1)
    #     print(x_start, y_start, x_final, y_final)
    #     if keyboard.is_pressed('4'):
    #         if (tic_x_start == False):
    #             x_start=f
    #             tic_x_start =True
    #     if keyboard.is_pressed('2'):
    #         if (tic_y_start == False):
    #              y_start = f
    #              tic_y_start = True
    #     if keyboard.is_pressed('6'):
    #         if (tic_x_final == False):
    #             x_final = f
    #             tic_x_final = True
    #     if keyboard.is_pressed('8'):
    #         if (tic_y_final == False):
    #             y_final = f
    #             tic_y_final = True
    #     if (tic_x_start == True and  tic_y_start == True and  tic_x_final == True and  tic_y_final == True):
    #         tic_key = True
    #         ax = plt.axes(xlim=(-x_start - 10, x_final + 10), ylim=(-y_start - 10, y_final + 10))
    #
    #         x = [-x_start, -x_start, x_final, x_final, -x_start]
    #         y = [-y_start, y_final, y_final, -y_start, -y_start]
    #
    #         print(x_start, y_start, x_final, y_final)

    plt.clf()


    frames = pipe.wait_for_frames()
    pose = frames.get_pose_frame()
    data = pose.get_pose_data()

    xlog.append(x_current)
    ylog.append(y_current)
    #
    # plt.scatter(x, y, c= 'r')
    # plt.plot(x, y, 'r--')
    plt.grid()
    plt.xlabel('X_axis')
    plt.ylabel('Y_axis')
    plt.title('Work_Space')

    plt.scatter(xlog, ylog, marker= 's', s = 1275, c = 'b')
    plt.scatter(x_current, y_current, marker='s', s=1275, c='g')

    plt.draw()

    x_current = -data.translation.x * 1000
    y_current = data.translation.z * 1000

    xlog.append(x_current)
    ylog.append(y_current)

    plt.pause(0.001)

