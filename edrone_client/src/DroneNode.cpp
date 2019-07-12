/*
BSD 2-Clause License

Copyright (c) 2019, Simranjeet Singh
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <edrone_client/edrone_services.h>
#include <geometry_msgs/PoseArray.h>
#include <sys/time.h>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <pthread.h>
#include <unistd.h>
#include <eyantra_drone/Common.h>
#include <eyantra_drone/Protocol.h>
#include <edrone_client/edrone_msgs.h>

#define PORT 23

using namespace std;

bool isSocketCreate=false;

Communication com;
Protocol pro;

ros::ServiceClient serviceClient;
edrone_client::edrone_services service;

int userRC[8]={1500,1500,1500,1500,1000,1000,1000,1000};

void *createSocket(void *threadid){
  isSocketCreate=com.connectSock();
  pthread_exit(NULL);
};

void *writeFunction(void *threadid){
  std::vector<int> requests;
  requests.push_back(MSP_RC);
  requests.push_back(MSP_ATTITUDE);
  requests.push_back(MSP_RAW_IMU);
  requests.push_back(MSP_ALTITUDE);
  requests.push_back(MSP_ANALOG);


  while(1)
  {
    pro.sendRequestMSP_SET_RAW_RC(userRC);
    pro.sendRequestMSP_GET_DEBUG(requests);
    usleep(22000);
  }
  pthread_exit(NULL);
}

void *readFunction(void *threadid){
  do 
  {
    com.readFrame();
    //usleep(5);
  }
  while(1);
  pthread_exit(NULL);
}

void *serviceFunction(void *threadid){
  while (1) 
  {
      if (serviceClient.call(service))
      {

       service.request.accX=accX;
       service.request.accY=accY;
       service.request.accZ=accZ;
       service.request.gyroX=gyroX;
       service.request.gyroY=gyroY;
       service.request.gyroZ=gyroZ;
       service.request.magX=magX;
       service.request.magY=magY;
       service.request.magZ=magZ;
       service.request.roll=roll;
       service.request.pitch=pitch;
       service.request.yaw=yaw;
       service.request.alt=alt;
       service.request.battery=battery;
       service.request.rssi=rssi;
      }
  }
  pthread_exit(NULL);
}

void Callback(const edrone_client::edrone_msgs::ConstPtr& msg){
   userRC[0] = msg->rcRoll;
   userRC[1] = msg->rcPitch;
   userRC[2] = msg->rcThrottle;
   userRC[3] = msg->rcYaw;
   userRC[4] = msg->rcAUX1;
   userRC[5] = msg->rcAUX2;
   userRC[6] = msg->rcAUX3;
   userRC[7] = msg->rcAUX4;
 // cout << "roll = " << userRC[0] << "pitch = " <<userRC[1] << "Throttle = "<< userRC[2]  << endl;
}

int main(int argc, char **argv){
    pthread_t thread, readThread, writeThread, serviceThread;
    int rc;
    ros::init(argc, argv, "DroneNode");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("drone_command", 1000, Callback);
    rc = pthread_create(&thread, NULL, createSocket, 	(void *)1);
    if (rc)
    {
     cout << "Error:unable to create communication thread," << rc << endl;
     exit(-1);
    }
    pthread_join( thread, NULL);

    if(isSocketCreate)
    {
      //cout << "main() : creating write thread, " << i << endl;
      rc = pthread_create(&writeThread, NULL, writeFunction, 	(void *)2);
      if (rc)
      {
        cout << "Error:unable to create write thread," << rc << endl;
        exit(-1);
      }
      // cout << "main() : creating read thread, " << i << endl;
      rc = pthread_create(&readThread, NULL, readFunction, 	(void *)3);
      if (rc)
      {
        cout << "Error:unable to read create thread," << rc << endl;
        exit(-1);
      }

      serviceClient = n.serviceClient<edrone_client::edrone_services>("droneService",true);
      //cout << "main() : creating service thread, " << i << endl;
      rc = pthread_create(&serviceThread, NULL, serviceFunction, 	(void *)4);
      
      if (rc)
      {
        cout << "Error:unable to service create thread," << rc << endl;
        exit(-1);
      }
    } 
  
    ros::spin();
    return 0;
}