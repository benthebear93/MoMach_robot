import numpy as np
import matplotlib.pyplot as plt
import mscl
from collections import deque
import random
import matplotlib.animation as animation
import matplotlib.pyplot as plt
from itertools import count
COM_PORT = "COM3"
scaled_X = []
scaled_Y = []
scaled_Z = []
gyro_X = []
gyro_Y = []
gyro_Z = []

Z_val = []
Y_val = []
X_val = []
gX_val = []
gY_val = []
gZ_val = []

x_val = []
index = count()
def scaled_XYZ_data(name, coord):
    if name[-1] == "X":
        if name[6] == "A":
            scaled_X.append(coord)
        else:
            gyro_X.append(coord)
    elif name[-1] == "Y":
        if name[6] == "A":
            scaled_Y.append(coord)
        else:
            gyro_Y.append(coord)
    else:
        if name[6] == "A":
            scaled_Z.append(coord)
        else:
            gyro_Z.append(coord)
    #print("scaled_X :", scaled_X)

def getSensorData():
    connection = mscl.Connection.Serial(COM_PORT, 115200)
    node = mscl.InertialNode(connection)
    success = node.ping()
    print("node init succese ? :", success)
    node.setToIdle()

    #activeChs = node.getActiveChannelFields(mscl.MipTypes.CLASS_AHRS_IMU)

    ahrsImuChs = mscl.MipChannels()
    ahrsImuChs.append(mscl.MipChannel(mscl.MipTypes.CH_FIELD_SENSOR_SCALED_ACCEL_VEC, mscl.SampleRate.Hertz(100)))
    ahrsImuChs.append(mscl.MipChannel(mscl.MipTypes.CH_FIELD_SENSOR_SCALED_GYRO_VEC, mscl.SampleRate.Hertz(100)))

    # set the active channels for the AHRS/IMU class on the Node
    node.setActiveChannelFields(mscl.MipTypes.CLASS_AHRS_IMU, ahrsImuChs)

    # start sampling on the AHRS/IMU, GNSS class of the Node
    node.enableDataStream(mscl.MipTypes.CLASS_AHRS_IMU)
    node.enableDataStream(mscl.MipTypes.CLASS_ESTFILTER)
    node.resume()
    while True:
        # get all the packets that have been collected, with a timeout of 500 milliseconds
        packets = node.getDataPackets(100)
        # print("packet data type :", type(packets))
        for packet in packets:
            packet.descriptorSet()  # the descriptor set of the packet
            # packet.timestamp()  # the PC time when this packet was received
            # get all of the points in the packet
            points = packet.data()  # def data(self): return _mscl.MipDataPacket_data(self)
            for dataPoint in points:
                dataPoint.channelName()  # the name of the channel for this point
                #print("datapoints channel Name :", type(dataPoint.channelName()))
                dataPoint.storedAs()  # the ValueType that the data is stored as
                dataPoint.as_float()  # get the value as a float
                #print(dataPoint.channelName(), " :", dataPoint.as_float())
                scaled_XYZ_data(dataPoint.channelName(), dataPoint.as_float())
        if(len(scaled_X) > 1000):
            break

def animate(i):
    x_val.append(next(index))

    X_val.append(scaled_X.popleft())
    Y_val.append(scaled_Y.popleft())
    Z_val.append(scaled_Z.popleft())
    gX_val.append(gyro_X.popleft())
    gY_val.append(gyro_Y.popleft())
    gZ_val.append(gyro_Z.popleft())
    fig, ax = plt.subplots()
    plt.cla()

    ax.plot(x_val, X_val, label='scaled x ')
    ax.plot(x_val, Y_val, label='scaled y')
    ax.plot(x_val, Z_val, label='scaled z')

    ax.plot(x_val, gX_val, label='gyro x')
    ax.plot(x_val, gY_val, label='gyro y')
    ax.plot(x_val, gZ_val, label='gyro z')
    ax.set_title('IMU Sensor')
    #ax.set_xlable('X')
    #ax.set_ylable('Y')
    ax.legend(loc='upper left')
    plt.tight_layout()

if __name__ == "__main__":
    getSensorData()
    scaled_X = deque(scaled_X)
    scaled_Y = deque(scaled_Y)
    scaled_Z = deque(scaled_Z)
    gyro_X = deque(gyro_X)
    gyro_Y = deque(gyro_Y)
    gyro_Z = deque(gyro_Z)

    ani = animation.FuncAnimation(plt.gcf(), animate, interval=1)
    plt.tight_layout()
    plt.show()
