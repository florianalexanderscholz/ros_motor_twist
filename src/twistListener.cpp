#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include <stdio.h>
#include <fcntl.h> 

int fdx, fdy;

void chatterCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
  double xi = (msg->angular.z) * 152.55/* * 1.8*/;
  double yi = (msg->linear.x)  * 300/* * 1.8*/; 
/* 152.55 300 */
  printf("new\n");
/* 76 */

  unsigned char x,y; 
//  int fdx, fdy; 
 
  if(xi >= 500) xi = 499;
  if(xi <= (-500)) xi = -499;
  if(yi >= 500) yi = 499;
  if(yi <=(-500)) yi = -499;
  printf("%d %d (xi,yi)\n");

  /* Magic formula */
  x = 43+(xi+500.0)*184.0/1000.0;
  y = 43+(yi+500.0)*184.0/1000.0;
  
  ROS_INFO("xi: [%lf] yi: [%lf] Coord - x: [%x] y: [%x]", xi,yi, x,y);

  //Nullwerte filtern 
  if( x == 0x00 || y == 0x00){
  	return;
  }
  
  write( fdx, &x, sizeof(x));
  write( fdy, &y, sizeof(y));
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "listener");
  fdx = open("/dev/dac-1", O_WRONLY);
  fdy = open("/dev/dac-2", O_WRONLY);
  if(fdx < 0 || fdy < 0) {
    perror("/dev/dac-x/y");
    
  }
  else {
    ros::NodeHandle n;

    ros::Subscriber sub = n.subscribe("cmd_vel", 1000000, chatterCallback);

    ros::spin();
  }
//  close_it:
  close(fdx);
  close(fdy);
  return 0;
}
