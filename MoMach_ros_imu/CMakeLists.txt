cmake_minimum_required(VERSION 2.8.3)
project(MoMach_ros_imu)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  nav_msgs
  roscpp
  roslint
  rospy
  sensor_msgs
  std_msgs
  tf2
  tf2_ros
)
find_package(cmake_modules REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Eigen REQUIRED)
include_directories(${Eigen_INCLUDE_DIRS})


find_library(MSCL_LIB_PATH NAMES libmscl.so PATHS "/usr/share/c++-mscl" DOC "MSCL Library" NO_DEFAULT_PATH)
find_library(BOOST_LIB_PATH NAMES libboost_chrono.so PATHS "/usr/share/c++-mscl/Boost/lib" NO_DEFAULT_PATH)
set(MSCL_INC_PATH "/usr/share/c++-mscl/source")
set(BOOST_INC_PATH "/usr/share/c++-mscl/Boost/include")

set(MSCL_LIB "usr/share/c++-mscl")
add_definitions(-std=c++11)

set(CMAKE_C_FLAGS "-Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-format -fno-builtin-memcpy")

###################################
## catkin specific configuration ##
###################################
catkin_package(
  INCLUDE_DIRS
    include
  CATKIN_DEPENDS
    roscpp
    tf2
    tf2_ros
    std_msgs
    geometry_msgs
    sensor_msgs
    nav_msgs  
  DEPENDS Eigen
)
##################

include_directories(include/${PROJECT_NAME} ${MIPLIBINC} ${MIPUTILINC} ${MSCL_INC_PATH} ${MSCL_LIB} ${MSCL_INC} ${catkin_INCLUDE_DIRS})

add_library(MoMach_ros_imu
  src/gyro_test.cpp
)
# Executables
add_executable(Imu_acc_pub src/gyro_test.cpp
  src/main.cpp)
target_link_libraries(Imu_acc_pub ${catkin_LIBRARIES} ${Eigen_LIBRARIES})
add_dependencies(Imu_acc_pub MoMach_ros_imu_gencpp)

target_link_libraries(MoMach_ros_imu
  ${MSCL_LIB_PATH}
  ${catkin_LIBRARIES}
  ${Eigen_LIBRARIES}
)
target_link_libraries(Imu_acc_pub
  MoMach_ros_imu
  ${catkin_LIBRARIES}
  ${Eigen_LIBRARIES}
)

include_directories(${catkin_INCLUDE_DIRS}
    include)

###########
## Build ##
###########
