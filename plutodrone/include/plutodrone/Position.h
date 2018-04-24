
#include "ros/ros.h"
#include <geometry_msgs/PoseArray.h>


extern double deltaT;

class Position{


public:

void positionCallback(const geometry_msgs::PoseArray::ConstPtr& msg);

void setPoseX(float x);
void setPoseY(float y);
void setPoseZ(float z);
float getPoseX();
float getPoseY();
float getPoseZ();
double getlastPositionTime();
bool isPositionTimeOut();






private:

float poseX;
float poseY;
float poseZ;
int *posarray;
double lastPostionTime;














};
