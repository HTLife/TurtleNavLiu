#include <iostream>

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include "nav_msgs/OccupancyGrid.h"

#include <sensor_msgs/Joy.h>
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"
#include "ros/console.h"


class TurtlebotTeleop
{
    public:
        TurtlebotTeleop();


        void publish();

    private:
        ros::NodeHandle ph_, nh_;

        int linear_, angular_, deadman_axis_;
        double l_scale_, a_scale_;
        ros::Publisher vel_pub_;
        ros::Subscriber joy_sub_;

        geometry_msgs::Twist last_published_;
        boost::mutex publish_mutex_;
        bool deadman_pressed_;
        bool zero_twist_published_;
        ros::Timer timer_;

};

TurtlebotTeleop::TurtlebotTeleop():
    ph_("~"),
    linear_(5),
    angular_(0),
    deadman_axis_(4),
    l_scale_(0.3),
    a_scale_(0.9)
{
    ph_.param("axis_linear", linear_, linear_);
    ph_.param("axis_angular", angular_, angular_);
    ph_.param("axis_deadman", deadman_axis_, deadman_axis_);
    ph_.param("scale_angular", a_scale_, a_scale_);
    ph_.param("scale_linear", l_scale_, l_scale_);

    deadman_pressed_ = false;
    zero_twist_published_ = false;
    
    //vel_pub_ = ph_.advertise<geometry_msgs::Twist>("cmd_vel", 1, true);
    vel_pub_ = ph_.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop", 1, true);

    //joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &TurtlebotTeleop::joyCallback, this);

    //timer_ = nh_.createTimer(ros::Duration(0.1), boost::bind(&TurtlebotTeleop::publish, this));
}


void TurtlebotTeleop::publish()
{
    geometry_msgs::Twist vel;
    vel.angular.z = 0; //a_scale_*joy->axes[angular_];
    vel.linear.x = 5; //l_scale_*joy->axes[linear_];
    last_published_ = vel;

    vel_pub_.publish(last_published_);
}

void chatterCallback(const nav_msgs::OccupancyGrid::ConstPtr& msg)
{
    std_msgs::Header header = msg->header;
    nav_msgs::MapMetaData info = msg->info;
    ROS_INFO("Got map %d %d", info.width, info.height);

}

int main(int argc, char** argv)
{//cmd_vel_mux/input/teleop
    ros::init(argc, argv, "keyboard");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("map", 1000, chatterCallback);
    //ros::init(argc, argv, "turtlebot_teleop");

    TurtlebotTeleop turtlebot_teleop;
    
    ros::Rate loop_rate(10);
    while(1)
    {
        
        turtlebot_teleop.publish();

        //ros::spin();
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
