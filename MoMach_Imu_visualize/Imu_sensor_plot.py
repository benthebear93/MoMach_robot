import sys
import os
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5 import QtCore
import numpy as np
import matplotlib
matplotlib.use("Qt5Agg")
from matplotlib.figure import Figure
from matplotlib.animation import TimedAnimation
from matplotlib.lines import Line2D
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
import time
import threading
import mscl
from itertools import count
COM_PORT = "COM3"
x_x_axis = []
x_y_axis = []
x_z_axis = []
scaled_x = []
scaled_y = []
scaled_z = []

class CustomMainWindow(QMainWindow):
    def __init__(self):
        super(CustomMainWindow, self).__init__()
        # Define the geometry of the main window
        self.setGeometry(300, 300, 800, 400)
        self.setWindowTitle("my first window")
        # Create FRAME_A
        self.FRAME_A = QFrame(self)
        self.FRAME_A.setStyleSheet("QWidget { background-color: %s }" % QColor(210, 210, 235, 255).name())
        self.LAYOUT_A = QGridLayout()
        self.FRAME_A.setLayout(self.LAYOUT_A)
        self.setCentralWidget(self.FRAME_A)
        # Place the zoom button
        self.zoomBtn = QPushButton(text = 'zoom in')
        self.zoomBtn.setFixedSize(100, 50)
        self.zoomBtn.clicked.connect(self.zoomBtnAction)

        self.zoomBtn2 = QPushButton(text = 'zoom out')
        self.zoomBtn2.setFixedSize(100, 50)
        self.zoomBtn2.clicked.connect(self.zoomBtnAction2)


        self.textLabel = QLabel("Test : ", self)
        self.textLabel.move(20, 20)

        self.label = QLabel("", self)
        self.label.move(80, 20)
        self.label.resize(150, 30)

        self.LAYOUT_A.addWidget(self.zoomBtn, *(0, 0))
        self.LAYOUT_A.addWidget(self.zoomBtn2, *(0, 1))
        # Place the matplotlib figure
        self.myFig = CustomFigCanvas(0)
        self.myFig2 = CustomFigCanvas(1)
        self.myFig3 = CustomFigCanvas(2)
        self.LAYOUT_A.addWidget(self.myFig, *(0, 2))
        self.LAYOUT_A.addWidget(self.myFig2, *(1, 2))
        self.LAYOUT_A.addWidget(self.myFig3, *(2, 2))
        # Add the callbackfunc to ..
        myDataLoop = threading.Thread(name='myDataLoop', target=dataSendLoop, daemon=True,
                                      args=(self.addData_callbackFunc, self.addData_callbackFunc2, self.addData_callbackFunc3 ))
        myDataLoop.start()
        self.show()
        return

    def zoomBtnAction(self):
        print("zoom in")
        self.myFig.zoomIn(0.5)
        return

    def zoomBtnAction2(self):
        print("zoom out")
        self.myFig.zoomOut(0.5)

    def addData_callbackFunc(self, value):
        #print("Add data: " + str(value))
        self.myFig.addData(value)
        return

    def addData_callbackFunc2(self, value):
        # print("Add data: " + str(value))
        self.myFig2.addData(value)
        return
    def addData_callbackFunc3(self, value):
        # print("Add data: " + str(value))
        self.myFig3.addData(value)
        return

''' End Class '''


class CustomFigCanvas(FigureCanvas, TimedAnimation):
    def __init__(self, num):
        self.fig_name = ['gyro_X', 'gyro_Y', 'gyro_Z']
        self.addedData = []
        print(matplotlib.__version__)
        # The data
        self.xlim = 200
        self.n = np.linspace(0, self.xlim - 1, self.xlim)
        print(self.n)
        self.y = (self.n * 0.0) + 50
        # The window
        self.fig = Figure(figsize=(5,5), dpi=100)
        self.ax1 = self.fig.add_subplot(111)
        # self.ax1 settings
        self.ax1.set_xlabel('time')
        self.ax1.set_ylabel(self.fig_name[num])
        self.line1 = Line2D([], [], color='blue', linewidth=1)
        self.line1_tail = Line2D([], [], color='red', linewidth=1)
        self.line1_head = Line2D([], [], color='red', marker='o', markeredgecolor='r')
        self.ax1.add_line(self.line1)
        self.ax1.add_line(self.line1_tail)
        self.ax1.add_line(self.line1_head)
        self.ax1.set_xlim(0, self.xlim - 1)
        self.ax1.set_ylim(-1, 1)
        FigureCanvas.__init__(self, self.fig)
        TimedAnimation.__init__(self, self.fig, interval=50, blit = True)
        return

    def new_frame_seq(self):
        return iter(range(self.n.size))

    def _init_draw(self):
        lines = [self.line1, self.line1_tail, self.line1_head]
        for l in lines:
            l.set_data([], [])
        return

    def addData(self, value):
        self.addedData.append(value)
        return

    def zoomIn(self, value):
        bottom = self.ax1.get_ylim()[0]
        top = self.ax1.get_ylim()[1]
        bottom += value
        top -= value
        self.ax1.set_ylim(bottom,top)
        self.draw()
        return

    def zoomOut(self, value):
        bottom = self.ax1.get_ylim()[0]
        top = self.ax1.get_ylim()[1]
        bottom -= value
        top += value
        self.ax1.set_ylim(bottom,top)
        self.draw()
        return

    def _step(self, *args):
        # Extends the _step() method for the TimedAnimation class.
        try:
            TimedAnimation._step(self, *args)
        except Exception as e:
            self.abc += 1
            print(str(self.abc))
            TimedAnimation._stop(self)
            pass
        return

    def _draw_frame(self, framedata):
        margin = 2
        while(len(self.addedData) > 0):
            self.y = np.roll(self.y, -1)
            self.y[-1] = self.addedData[0]
            del(self.addedData[0])

        self.line1.set_data(self.n[ 0 : self.n.size - margin ], self.y[ 0 : self.n.size - margin ])
        self.line1_tail.set_data(np.append(self.n[-10:-1 - margin], self.n[-1 - margin]), np.append(self.y[-10:-1 - margin], self.y[-1 - margin]))
        self.line1_head.set_data(self.n[-1 - margin], self.y[-1 - margin])
        self._drawn_artists = [self.line1, self.line1_tail, self.line1_head]
        return

''' End Class '''

class Communicate(QObject):
    data_signal = pyqtSignal(float)

''' End Class '''


def dataSendLoop(addData_callbackFunc, addData_callbackFunc2, addData_callbackFunc3):
    connection = mscl.Connection.Serial(COM_PORT, 115200)
    # Setup the signal-slot mechanism.
    mySrc = Communicate()
    mySrc.data_signal.connect(addData_callbackFunc)
    mySrc2 = Communicate()
    mySrc2.data_signal.connect(addData_callbackFunc2)
    mySrc3 = Communicate()
    mySrc3.data_signal.connect(addData_callbackFunc3)

    # connect to IMU
    node = mscl.InertialNode(connection)
    success = node.ping()
    print("node init working ? :", success)
    node.setToIdle()

    ahrsImuChs = mscl.MipChannels()
    ahrsImuChs.append(mscl.MipChannel(mscl.MipTypes.CH_FIELD_SENSOR_SCALED_ACCEL_VEC, mscl.SampleRate.Hertz(100)))
    ahrsImuChs.append(mscl.MipChannel(mscl.MipTypes.CH_FIELD_SENSOR_SCALED_GYRO_VEC, mscl.SampleRate.Hertz(100)))

    node.setActiveChannelFields(mscl.MipTypes.CLASS_AHRS_IMU, ahrsImuChs)

    # start sampling on the AHRS/IMU, GNSS class of the Node
    node.enableDataStream(mscl.MipTypes.CLASS_AHRS_IMU)
    node.enableDataStream(mscl.MipTypes.CLASS_ESTFILTER)
    i = 0
    j = 0
    k = 0
    while True:
        packets = node.getDataPackets(500) # get packets with time out of 500 milliseconds
        for packet in packets:
            points = packet.data()
            for dp in points:
                if(dp.channelName() =='estLinearAccelZ' or dp.channelName()=='estLinearAccelX' or dp.channelName() =='estLinearAccelY'):
                    break
                else:
                    if dp.channelName() == 'scaledGyroX':
                        scaled_x.append(dp.as_float())
                        x_x_axis.append(i)
                        mySrc.data_signal.emit(scaled_x[i])  # <- Here you emit a signal!
                        i += 1
                    elif dp.channelName() == 'scaledGyroY':
                        scaled_y.append(dp.as_float())
                        x_y_axis.append(j)
                        mySrc2.data_signal.emit(scaled_y[j])
                        j += 1
                    if dp.channelName()== 'scaledGyroZ':
                        scaled_z.append(dp.as_float())
                        x_z_axis.append(k)
                        mySrc3.data_signal.emit(scaled_z[k])
                        k += 1

if __name__== '__main__':
    app = QApplication(sys.argv)
    QApplication.setStyle(QStyleFactory.create('Plastique'))
    myGUI = CustomMainWindow()
    sys.exit(app.exec_())