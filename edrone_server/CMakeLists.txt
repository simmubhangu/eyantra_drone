cmake_minimum_required(VERSION 2.8.3)
project(edrone_server)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  edrone_client
  message_generation

)

catkin_package(
  #INCLUDE_DIRS include
  LIBRARIES edrone_client
  CATKIN_DEPENDS roscpp rospy std_msgs
  #DEPENDS system_lib
  message_runtime
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(edroneserver src/drone_server.cpp)
target_link_libraries(edroneserver ${catkin_LIBRARIES})
add_dependencies(edroneserver edrone_client_gencpp)


