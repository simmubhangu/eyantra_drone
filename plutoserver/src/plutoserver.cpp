#include "ros/ros.h"
#include "plutodrone/PlutoPilot.h"

bool myFunction(plutodrone::PlutoPilot::Request  &req,
         plutodrone::PlutoPilot::Response &res)
{
  ROS_INFO("Ax=%f, Ay=%f, Az=%f", req.accX, req.accY, req.accZ);
  ROS_INFO("Gx=%f, Gy=%f, Gz=%f", req.gyroX, req.gyroY, req.gyroZ);
  ROS_INFO("Mx=%f, My=%f, Mz=%f", req.magX, req.magY, req.magZ);

  
  ROS_INFO("roll=%i, pitch=%i, yaw=%i", req.roll, req.pitch, req.yaw);
  ROS_INFO("altitiude=%f", req.alt);
      res.rcAUX1=1800;
    
  ROS_INFO("sending back response: [%ld]", (long int)res.rcAUX1);
  return true;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "PlutoData");
  ros::NodeHandle n;

  ros::ServiceServer service = n.advertiseService("PlutoService", myFunction);
  printf("Ready to Provide Pluto Service\n");
  ros::spin();

  return 0;
}
