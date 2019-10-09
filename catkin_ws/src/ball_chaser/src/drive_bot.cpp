#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "ball_chaser/DriveToTarget.h"

// ROS::Publisher motor commands; Global, prefer pub-sub class
ros::Publisher motor_command_publisher;

bool handle_drive_request(ball_chaser::DriveToTarget::Request& req,
    ball_chaser::DriveToTarget::Response& res)
{
	ROS_INFO("GoToPositionRequest received - j1:%1.2f, j2:%1.2f", (float)req.linear_x, (float)req.angular_z);
	
	// Creating a motor_command object of type geometry_msgs::Twist		
	geometry_msgs::Twist motor_command;
			
	// Set wheel velocities, forward [0.5, 0.0]
	motor_command.linear.x = req.linear_x;
	motor_command.angular.z = req.angular_z;
			
	// Publishing angles to drive the robot
	motor_command_publisher.publish(motor_command);
		
	// Return a response message
    res.msg_feedback = "Velocities are - Linear: " + std::to_string(req.linear_x) + " , Angular: " + std::to_string(req.angular_z);

    ROS_INFO_STREAM(res.msg_feedback);

 return true;
}

int main(int argc, char** argv)
{
    // Initialize a ROS node
    ros::init(argc, argv, "drive_bot");

    // Create a ROS NodeHandle object
    ros::NodeHandle n;

    // Inform ROS master that we will be publishing a message of type geometry_msgs::Twist on the robot actuation topic with a publishing queue size of 10
    motor_command_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    // Drive /ball_chaser/command_robot service with a handle_drive_request callback function
    ros::ServiceServer service = n.advertiseService("/ball_chaser/command_robot", handle_drive_request);
	ROS_INFO("Ready to send driving commands");

    // ROS communication events
    ros::spin();

    return 0;
}
