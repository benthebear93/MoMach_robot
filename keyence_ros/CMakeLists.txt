cmake_minimum_required(VERSION 2.8.3)
project(keyence_ros)

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs tf tf2_ros pcl_conversions pcl_ros genmsg sensor_msgs message_generation)

## Declare ROS messages and services
add_message_files(FILES keyence_laser.msg)
#add_service_files(FILES AddTwoInts.srv)

## Generate added messages and services
generate_messages(DEPENDENCIES std_msgs)

## Declare a catkin package
catkin_package(INCLUDE_DIRS CATKIN_DEPENDS roscpp pcl_conversions pcl_ros)

## Build talker and listener
include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(keyence_output src/keyence_output.cpp)
target_link_libraries(keyence_output ${catkin_LIBRARIES})
add_dependencies(keyence_output keyence_output_generate_messages_cpp)

add_executable(keyence_laser src/keyence_laser.cpp)
target_link_libraries(keyence_laser ${catkin_LIBRARIES})
add_dependencies(keyence_laser keyence_laser_generate_messages_cpp)

add_executable(keyence_profile src/keyence_profile.cpp)
target_link_libraries(keyence_profile ${catkin_LIBRARIES})
add_dependencies(keyence_profile keyence_profile_generate_messages_cpp)
