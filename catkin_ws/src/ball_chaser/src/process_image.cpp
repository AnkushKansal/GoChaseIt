#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("BOT IS ABOUT TO MOVE");

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the command_robot service and pass the requested motor commands
    if (!client.call(srv))
        ROS_ERROR("Failed to call service /ball_chaser/command_robot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
	
    bool found_ball = false;   	
    int rows = 0;
    int steps = 0; //step is a full row.
    int index = 0;
	

    //Identify the object.
    for (rows = 0; rows < img.height && found_ball == false; rows++)
    {
        for (steps = 0; steps < img.step && found_ball == false; ++steps)
        {   
            index = (rows*img.step)+steps; //assuming image pixels in row major order.
            
            if (img.data[index] == white_pixel && img.data[index+1] == white_pixel && img.data[index+2] == white_pixel)
            {   
                found_ball = true;            
            }
		}
    }
	
	//We identified the object and now find its position in image. Identifying if this pixel falls in the left, mid, or right side of the image
    if (found_ball)
    {
        int imgThird = img.width/3; //Divided the image in 3 parts. Left, middle, right
        int col = steps/3;

        if (col < imgThird) 
        {
	    ROS_INFO("MOVE LEFT");
            drive_robot(0.1, 0.5);
        } 

	//In middle
        else if (col >= imgThird && col <= 2*imgThird)
        {
	    ROS_INFO("STAY IN MID");
            drive_robot(0.5, 0.0);
            
        }

        else if (col > 2*imgThird)
        {
	    ROS_INFO("MOVE RIGHT");
            drive_robot(0.1, -0.5);
            
        }
    }

    else 
    {
        //Stopping when there's no white ball seen by the camera. No Perception of ball
	ROS_INFO("STOP");
        drive_robot(0.0, 0.0);        
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
