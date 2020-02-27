import numpy as np
import matplotlib.pyplot as plt
import mscl
import matplotlib.animation as ani
from collections import deque

COM_PORT = "COM3"
data_X = []
data_Y = []
data_Z = []
MAX_X = 100   #width of graph
MAX_Y = 5  #height of graph

def scaled_XYZ_data(name, coord):
    if name[-1] == "X":
        data_X.append(coord)
    elif name[-1] == "Y":
        data_Y.append(coord)
    else:
        data_Z.append(coord)
    print("data_x :", data_X)

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
    fig, ax = plt.subplots(1,1)
    ax.set_xlim(0,100)
    ax.set_ylim(-2, 2)

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
                # print(dataPoint.as_float())
                scaled_XYZ_data(dataPoint.channelName(), dataPoint.as_float())

if __name__ == "__main__":
    getSensorData()
