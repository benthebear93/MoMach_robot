import numpy as np
import matplotlib.pyplot as plt
import mscl
from collections import deque
import random
import matplotlib.animation as animation
import matplotlib.pyplot as plt
from itertools import count
COM_PORT = "COM3"

x_x_axis = []
x_y_axis = []
x_z_axis = []
scaled_x = []
scaled_y = []
scaled_z = []

def getSensorData():
    connection = mscl.Connection.Serial(COM_PORT, 115200)
    node = mscl.InertialNode(connection)
    success = node.ping()
    print("node init succese ? :", success)
    node.setToIdle()

    if node.features().supportsCategory(mscl.MipTypes.CLASS_AHRS_IMU):
        #get a list of the AHRS/IMU channels currently active on the Node
        ahrsImuActiveChs = node.getActiveChannelFields(mscl.MipTypes.CLASS_AHRS_IMU)

        print ("AHRS/IMU Channels")
        print ("-----------------")
        for ch in ahrsImuActiveChs:
            print ("Channel Field:", ch.channelField())
            print ("Sample Rate:", ch.sampleRate().prettyStr())

    ahrsImuChs = mscl.MipChannels()
    ahrsImuChs.append(mscl.MipChannel(mscl.MipTypes.CH_FIELD_SENSOR_SCALED_ACCEL_VEC, mscl.SampleRate.Hertz(100)))
    ahrsImuChs.append(mscl.MipChannel(mscl.MipTypes.CH_FIELD_SENSOR_SCALED_GYRO_VEC, mscl.SampleRate.Hertz(100)))

    # set the active channels for the AHRS/IMU class on the Node
    node.setActiveChannelFields(mscl.MipTypes.CLASS_AHRS_IMU, ahrsImuChs)

    # start sampling on the AHRS/IMU, GNSS class of the Node
    node.enableDataStream(mscl.MipTypes.CLASS_AHRS_IMU)
    node.enableDataStream(mscl.MipTypes.CLASS_ESTFILTER)

    while True:
        # get all the packets that have been collected, with a timeout of 500 milliseconds
        packets = node.getDataPackets(0)
        # print("packet data type :", type(packets))
        i = 0
        for packet in packets:
            packet.descriptorSet()  # the descriptor set of the packet
            # a =packet.collectedTimestamp()  # the PC time when this packet was received
            # # get all of the points in the packet
            points = packet.data()  # def data(self): return _mscl.MipDataPacket_data(self)
            for dataPoint in points:
                if(dataPoint.channelName() =='estLinearAccelZ' or dataPoint.channelName()=='estLinearAccelX' or dataPoint.channelName() =='estLinearAccelY'):
                    break
                else:
                    print(dataPoint.channelName())
                    dataPoint.channelName()  # the name of the channel for this point
                    dataPoint.storedAs()  # the ValueType that the data is stored as
                    dataPoint.as_float()  # get the value as a float
                    if (dataPoint.channelName() == 'scaledGyroX'):
                        scaled_x.append(dataPoint.as_float())
                        x_x_axis.append(i)
                        i += 1
                    elif(dataPoint.channelName() == 'scaledAccelY'):
                        scaled_y.append(dataPoint.as_float())
                    else:
                        scaled_z.append(dataPoint.as_float())
                    print("list x : ", x_x_axis)
                    print("x len is :", len(x_x_axis), "y len is : ", len(scaled_x))
                    ax.plot(x_x_axis, scaled_x, color='b')
                    fig.canvas.draw()
                    ax.set_ylim([-0.1, 0.1])
                    #ax.set_xlim(left=max(0.0, i - 50.0), right=i + 50.0)
                    print("scaled x :", scaled_x)
                    print("scaled y :", scaled_y)
                    print("scaled z :", scaled_z)


if __name__ == "__main__":
    fig = plt.figure(figsize=(5, 5))
    ax = fig.add_subplot(111)
    fig.show()
    getSensorData()
