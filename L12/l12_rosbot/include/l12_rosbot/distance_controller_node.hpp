#pragma once
#include <chrono>
#include <functional>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>

namespace l12_rosbot {

// Drives the ROSbot straight ahead for one distance with a
// proportional controller. odometry_callback stores the start
// position and the position now. Every 50 ms (20 Hz),
// control_callback computes the error, the distance left to drive,
// and publishes a forward speed proportional to it. Within the
// tolerance, it stops.
class DistanceControllerNode : public rclcpp::Node {
  public:
    DistanceControllerNode()
        : rclcpp::Node{"distance_controller_node"},
          odometry_subscription_{
              create_subscription<nav_msgs::msg::Odometry>(
                  "odometry/filtered", 10,
                  std::bind(&DistanceControllerNode::odometry_callback,
                            this, std::placeholders::_1))},
          command_publisher_{
              create_publisher<geometry_msgs::msg::TwistStamped>(
                  "cmd_vel", 10)},
          control_timer_{create_wall_timer(
              std::chrono::milliseconds{50},
              std::bind(&DistanceControllerNode::control_callback,
                        this))} {}

  private:
    void odometry_callback(const nav_msgs::msg::Odometry &msg);
    void control_callback();
    void publish_command(double linear_speed);
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr
        odometry_subscription_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr
        command_publisher_;
    rclcpp::TimerBase::SharedPtr control_timer_;
    double start_x_{0.0};      // the start position, in m
    double start_y_{0.0};
    double x_{0.0};            // the position now, in m
    double y_{0.0};
    bool has_odometry_{false}; // true once odometry arrived
};

} // namespace l12_rosbot
