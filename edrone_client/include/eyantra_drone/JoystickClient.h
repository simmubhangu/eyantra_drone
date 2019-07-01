#include "ros/ros.h"
#include <sensor_msgs/Joy.h>


extern int localizationSate;

class JoystickClient{

public:

  void joystickCallback(const sensor_msgs::Joy::ConstPtr& msg);
  int rcJoystick[8]={1500,1500,1500,1500,1500,1250,1500,1200};



  int* getRcJoystick();








};
