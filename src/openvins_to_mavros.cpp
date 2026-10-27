#include <ros/ros.h>

#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>

class OpenVinsToMavros
{
public:
  OpenVinsToMavros()
  {
    ros::NodeHandle private_node("~");

    private_node.param<std::string>("input_topic", input_topic_, "/ov_msckf/odomimu");
    private_node.param<std::string>("output_topic", output_topic_, "/mavros/vision_pose/pose");

    pose_publisher_ = node_.advertise<geometry_msgs::PoseStamped>(output_topic_, 10);
    pose_subscriber_ = node_.subscribe(input_topic_, 10, &OpenVinsToMavros::poseCallback, this);

    ROS_INFO("OpenVINS input: %s", input_topic_.c_str());
    ROS_INFO("MAVROS pose output: %s", output_topic_.c_str());
  }

private:
  void poseCallback(const nav_msgs::Odometry::ConstPtr& odometry)
  {
    if (odometry->header.stamp.isZero())
    {
      ROS_WARN_THROTTLE(5.0, "OpenVINS pose has a zero timestamp; using current ROS time");
    }

    geometry_msgs::PoseStamped pose;
    pose.header = odometry->header;
    if (pose.header.stamp.isZero())
    {
      pose.header.stamp = ros::Time::now();
    }
    pose.pose = odometry->pose.pose;
    pose_publisher_.publish(pose);
  }

  ros::NodeHandle node_;
  ros::Subscriber pose_subscriber_;
  ros::Publisher pose_publisher_;
  std::string input_topic_;
  std::string output_topic_;
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "openvins_to_mavros");
  OpenVinsToMavros node;
  ros::spin();
  return 0;
}