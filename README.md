# MoMach_robot
Mobile Machining robot _ MHM_Lab(UNIST)

Imu sensor : Lord sensing systems 3DM-CX5-15

Linux Ubuntu Ver : 18.04

ROS Ver : Melodic  

---
**MoMach_Imu_visualize** 

1. visualize imu sensing data using matplot, tkinter (python).

   *reference code : https://stackoverflow.com/questions/30023763/how-to-make-an-interactive-2d-grid-in-a-window-in-python*

2. visualize imu sensing data using pyqt, matplot (python).

   *reference code : https://stackoverflow.com/questions/11874767/how-do-i-plot-in-real-time-in-a-while-loop-using-matplotlib*

   

**MoMach_ros_Imu**

1. Getting gyro & accelerometer sensor data to convert R,P,Y angle and publish by ROS.(C++)

2. Publish /tf, sensor_msgs/Imu for EKF and other packages.

**MoMach_path_visualize**

1. Path visualize with matplotlib(python)  
problem : Slow
---
## PLC to Pc Communication

PLC : Delta NC300  

PC : Window10, RPI4 Linux  

Protocol : Modbus TCP/IP Ethernet

1. **Delta_plc2pc** : Read Position data from CNC Controller. Based on Modbus TCP/IP Protocol (PC, C++)

2. **modbus_xy.py** :  Read Position data from CNC and convert to 32bit float. Based on Modbus TCP/IP Protocol(Rpi4, Python)
---
## Keyence laser sensor ##

Product : LJ-V7080 laser sensor

launch : **roslaunch keyence_ros keyence_toolsimul.launch**
1. bagfile play  

2. node /pid_simul init  

3. rqt_plot for /keyence_output_1/data & /tool_z_pos  

bagfiles : specimen height measurement data *(topic : /keyence_output_1/data)*

node : /pid_simul *(topic : /tool_z_pos)*, /tool_pos_pid_plot *(rqt_plot)*




