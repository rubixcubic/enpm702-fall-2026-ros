#pragma once
#include <chrono>
#include <functional>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>

namespace l12_rosbot {

// Turns the ROSbot on the spot to one heading with a proportional
// controller. odometry_callback stores the robot's heading. Every
// 50 ms (20 Hz), control_callback computes the error, the angle left
// to turn, and publishes an angular speed proportional to it. Within
// the tolerance, it stops.
class HeadingControllerNode : public rclcpp::Node {
  public:
    HeadingControllerNode()
        : rclcpp::Node{"heading_controller_node"},
          odometry_subscription_{
              create_subscription<nav_msgs::msg::Odometry>(
                  "odometry/filtered", 10,
                  std::bind(&HeadingControllerNode::odometry_callback,
                            this, std::placeholders::_1))},
          command_publisher_{
              create_publisher<geometry_msgs::msg::TwistStamped>(
                  "cmd_vel", 10)},
          control_timer_{create_wall_timer(
              std::chrono::milliseconds{50},
              std::bind(&HeadingControllerNode::control_callback,
                        this))} {}

  private:
    void odometry_callback(const nav_msgs::msg::Odometry &msg);
    void control_callback();
    void publish_command(double angular_speed);
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr
        odometry_subscription_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr
        command_publisher_;
    rclcpp::TimerBase::SharedPtr control_timer_;
    double yaw_{0.0};          // the robot's heading, in rad
    bool has_odometry_{false}; // true once odometry arrived
};

} // namespace l12_rosbot
