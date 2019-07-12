
/* BSD 2-Clause License

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
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */


#include <ros/ros.h>
#include <edrone_client/edrone_services.h>

bool myFunction(edrone_client::edrone_services::Request  &req,
         edrone_client::edrone_services::Response &res)
{
  ROS_INFO("Ax=%f, Ay=%f, Az=%f", req.accX, req.accY, req.accZ);
  ROS_INFO("Gx=%f, Gy=%f, Gz=%f", req.gyroX, req.gyroY, req.gyroZ);
  ROS_INFO("Mx=%f, My=%f, Mz=%f", req.magX, req.magY, req.magZ);

  
  ROS_INFO("roll=%i, pitch=%i, yaw=%i", req.roll, req.pitch, req.yaw);
  ROS_INFO("altitiude=%f", req.alt);
  ROS_INFO("battery=%f rssi=%i", req.battery,req.rssi);

      res.rcAUX1=1800;
    
  ROS_INFO("sending back response: [%ld]", (long int)res.rcAUX1);
  return true;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "DroneData");
  ros::NodeHandle n;

  ros::ServiceServer service = n.advertiseService("DroneService", myFunction);
  printf("Ready to Provide Drone Service\n");
  ros::spin();

  return 0;
}
