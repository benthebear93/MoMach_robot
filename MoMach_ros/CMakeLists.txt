cmake_minimum_required(VERSION 2.8.3)
project(MoMach_ros)

find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs genmsg)

add_message_files(FILES pos_stream.msg)

generate_messages(DEPENDENCIES std_msgs)

catkin_package()
include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(pos_stream src/pos_stream.cpp)
target_link_libraries(pos_stream ${catkin_LIBRARIES})
add_dependencies(pos_stream MoMach_ros_gencpp)

