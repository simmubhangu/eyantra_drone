#include "ros/ros.h"
#include "std_msgs/String.h"
#include "drone_client/drone_service.h"
#include <geometry_msgs/PoseArray.h>
#include <sys/time.h>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <pthread.h>
#include <unistd.h>
#include <eyantra_drone/Common.h>
#include <eyantra_drone/Protocol.h>
#include <drone_client/eyantra_drone.h>
#include <stdlib.h>
// #include <eyantra_drone/JoystickClient.h>
// #include <eyantra_drone/Position.h>
#include <string>

#define PORT 23

using namespace std;


bool isSocketCreate=false;

Communication com;
Protocol pro;

ros::ServiceClient serviceClient;
drone_client::drone_service service[2];

//9 elements for drone index connected in the network
int userRC[9]={1500,1500,1500,1500,1000,1000,1000,1000,0};

//vector of all string ips
vector <string> all_ips;

struct ip_struct
{
  int index;
  std::string ip;
};

void *createSocket(void *arg){
  
  struct ip_struct local_var = *(struct ip_struct *)arg;
  //cout<<local_ip.ip;
  isSocketCreate=com.connectMulSock(local_var.ip, local_var.index);
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
    pro.sendMulRequestMSP_SET_RAW_RC(userRC);
    pro.sendMulRequestMSP_GET_DEBUG(requests, userRC[8]);
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

void *readMulFunction(void *arg){

  struct ip_struct local_var = *(struct ip_struct *)arg;
  do 
  {
    com.readMulFrame(local_var.index);
    //usleep(5);
  }
  while(1);
  pthread_exit(NULL);
}

void *serviceFunction(void *arg){
  struct ip_struct local_var = *(struct ip_struct *)arg;
  while (1) 
  {
    if (serviceClient.call(service[local_var.index]))
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
      service[local_var.index].request.accX=accX;
      service[local_var.index].request.accY=accY;
      service[local_var.index].request.accZ=accZ;
      service[local_var.index].request.gyroX=gyroX;
      service[local_var.index].request.gyroY=gyroY;
      service[local_var.index].request.gyroZ=gyroZ;
      service[local_var.index].request.magX=magX;
      service[local_var.index].request.magY=magY;
      service[local_var.index].request.magZ=magZ;
      service[local_var.index].request.roll=roll;
      service[local_var.index].request.pitch=pitch;
      service[local_var.index].request.yaw=yaw;
      service[local_var.index].request.alt=alt;
      // cout << alt << endl;
      //usleep(1000);
    }
  }
 pthread_exit(NULL);
}

void Callback(const drone_client::eyantra_drone::ConstPtr& msg){
 userRC[0] = msg->rcRoll;
 userRC[1] = msg->rcPitch;
 userRC[2] = msg->rcThrottle;
 userRC[3] = msg->rcYaw;
 userRC[4] = msg->rcAUX1;
 userRC[5] = msg->rcAUX2;
 userRC[6] = msg->rcAUX3;
 userRC[7] = msg->rcAUX4;
 userRC[8] = msg->droneIndex;
 // cout << "roll = " << userRC[0] << "pitch = " <<userRC[1] << "Throttle = "<< userRC[2]  << endl;
}

int main(int argc, char **argv){
  
  ros::init(argc, argv, "droneswarm");

  //add IPs here  
  all_ips.push_back("192.168.43.151");
  //all_ips.push_back("192.168.43.249");  
  //all_ips.push_back("");

  unsigned int i = 0;
  
  //struct to pass to create thread
  struct ip_struct ipStructVar;
  
  //Topic name. Index gets appended in for loop
  char topic_name[] = "drone_command/ ";
  char service_name[] = "droneService ";

  ros::NodeHandle n;

  ros::Subscriber sub[all_ips.size()];

  //create thread
  pthread_t tds[all_ips.size()];
  //read thread
  pthread_t rds[all_ips.size()];
  //write thread
  pthread_t wds[all_ips.size()];
  //service thread
  pthread_t sds[all_ips.size()];

  int rc;
    
  for(i = 0; i < all_ips.size(); i++){
    cout<<"Start";
    
    //Add an index to the topic
    topic_name[strlen(topic_name)-1] = 0x30+i;
    //Add an index to the service
    service_name[strlen(service_name)-1] = 0x30+i;
    
    //Multiple topics is not really required since a single callback is being used
    //and droneMsg is modified to identify the drone by index starting from 0.
    //However, I've placed it to make it user friendly, because people are stupid!
    sub[i] = n.subscribe(topic_name, 1000, Callback);
    
    //Get the IP in the IP struct
    ipStructVar.index = i;
    ipStructVar.ip = all_ips[i];

    //create thread
    rc = pthread_create(&tds[i], NULL, createSocket, (void *) &ipStructVar);
    if (rc)
    {
      cout << "Error:unable to create communication thread," << rc << endl;
      exit(-1);
    }

    pthread_join( tds[i], NULL);
      
    if(isSocketCreate)
    {
      rc = pthread_create(&wds[i], NULL, writeFunction, (void *)2);
      if (rc)
      {
        cout << "Error:unable to create write thread," << rc << endl;
        exit(-1);
      }

      //Couldn't figure out how to get the data from drones!

      rc = pthread_create(&rds[i], NULL, readMulFunction, (void *) &ipStructVar);
        
      if (rc)
      {
        cout << "Error:unable to read create thread," << rc << endl;
        exit(-1);
      }

      cout<<service_name;

      serviceClient = n.serviceClient<drone_client::drone_service>(service_name,true);
      rc = pthread_create(&sds[i], NULL, serviceFunction, (void *) &ipStructVar);
        
      if (rc)
      {
        cout << "Error:unable to service create thread," << rc << endl;
        exit(-1);
      }
    }

    cout<<"End";
  }
    ros::spin();
    return 0;
}