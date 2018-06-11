#include "ros/ros.h"
#include "std_msgs/String.h"
#include "plutodrone/PlutoPilot.h"
#include <geometry_msgs/PoseArray.h>
#include <sys/time.h>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <pthread.h>
#include <unistd.h>
#include <plutodrone/Common.h>
#include <plutodrone/Protocol.h>
#include <plutodrone/PlutoMsg.h>
// #include <plutodrone/JoystickClient.h>
// #include <plutodrone/Position.h>


#define PORT 23

using namespace std;


bool isSocketCreate=false;

Communication com;
Protocol pro;

ros::ServiceClient serviceClient;
plutodrone::PlutoPilot service;

int userRC[8]={0,0,0,0,0,0,0,0};

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
       //  ROS_INFO("Sum: %ld", (long int)service.response.rcAUX1);
       // userRC[0]=service.response.rcRoll;
       // userRC[1]=service.response.rcPitch;
       // userRC[2]=service.response.rcThrottle;
       // userRC[3]=service.response.rcYaw;
       // userRC[4]=service.response.rcAUX1;
       // userRC[5]=service.response.rcAUX2;
       // userRC[6]=service.response.rcAUX3;
       // userRC[7]=service.response.rcAUX4;
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
       // cout << alt << endl;
    //usleep(1000);
  }
  }
 pthread_exit(NULL);
}

void Callback(const plutodrone::PlutoMsg::ConstPtr& msg){
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
    ros::init(argc, argv, "plutonode");
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

      serviceClient = n.serviceClient<plutodrone::PlutoPilot>("PlutoService",true);
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