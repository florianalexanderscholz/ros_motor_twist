/*
 * Copyright (c) 2018 Florian Scholz <florian90@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of mosquitto nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include <stdio.h>
#include <fcntl.h> 

int fdx, fdy;

void chatterCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
  /* The formulas are mathematically equivalent to the formulas from the masters thesis.
   * Please note that xi is equivalent to msg->angular.z * (500 / max_vel_x)
   * and yi is equivalent to msg->linear.x * (500 / max_vel_y)
   */
  double xi = (msg->angular.z) * 152.55;
  double yi = (msg->linear.x)  * 300; 
  
  unsigned char x,y; 
 
  if(xi >= 500) xi = 499;
  if(xi <= (-500)) xi = -499;
  if(yi >= 500) yi = 499;
  if(yi <=(-500)) yi = -499;
  printf("%d %d (xi,yi)\n");

  x = 43+(xi+500.0)*184.0/1000.0;
  y = 43+(yi+500.0)*184.0/1000.0;
  
  ROS_INFO("xi: [%lf] yi: [%lf] Coord - x: [%x] y: [%x]", xi,yi, x,y);

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
  
  close(fdx);
  close(fdy);
  return 0;
}
