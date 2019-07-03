cmake_minimum_required(VERSION 2.8.3)
project(edrone_client)

add_definitions(-std=c++11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -Wall -g -O4 -march=native -Wfatal-errors")

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
  cv_bridge
  image_transport

)

#find_package(SDL2 REQUIRED SDL2)

find_package(OpenCV REQUIRED)
message(STATUS "Using OpenCV version ${OpenCV_VERSION}")
find_package(Boost COMPONENTS program_options thread system REQUIRED)

add_message_files(
  FILES
  edrone_msgs.msg
)

add_service_files(
   FILES
    edrone_services.srv
 )

 generate_messages(
   DEPENDENCIES
   std_msgs
 )

catkin_package(
  DEPENDS libsdl-dev
  INCLUDE_DIRS include
  LIBRARIES edrone_client

 CATKIN_DEPENDS roscpp
 cv_bridge
 sensor_msgs
 rospy
 std_msgs
 message_runtime
)

include_directories(include)


add_library(edrone_client src/Communication.cpp src/Protocol.cpp)
target_link_libraries(edrone_client)


find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs cv_bridge image_transport genmsg)

catkin_package()



add_executable(edroneclient src/DroneNode.cpp)
target_link_libraries(edroneclient ${catkin_LIBRARIES} edrone_client)
add_dependencies(edroneclient edrone_client_generate_messages_cpp)

add_executable(edroneswarm src/DroneSwarm.cpp)
target_link_libraries(edroneswarm ${catkin_LIBRARIES} edrone_client)
add_dependencies(edroneswarm edrone_client_generate_messages_cpp)

add_executable(edronecam src/DroneCam.cpp)
target_link_libraries(edronecam ${catkin_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES} edrone_client)

include_directories(include ${catkin_INCLUDE_DIRS})

target_link_libraries(edronecam SDL2 "/usr/lib/x86_64-linux-gnu/libavcodec.so" "/usr/lib/x86_64-linux-gnu/libavformat.so" 
"/usr/lib/x86_64-linux-gnu/libavutil.so" "/usr/lib/x86_64-linux-gnu/libavdevice.so" 
"/usr/lib/x86_64-linux-gnu/libswscale.so" "${PROJECT_SOURCE_DIR}/lib/libLeweiLib.so")

SET_TARGET_PROPERTIES(edroneclient PROPERTIES LINKER_LANGUAGE CXX)
