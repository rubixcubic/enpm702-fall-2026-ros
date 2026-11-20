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
// The distance, the gain, the speed limit and the tolerance are
// parameters.
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
                        this))} {
        // Declare each parameter with its default value.
        declare_parameter("target_distance_m", 1.0);
        declare_parameter("kp_linear", 0.8);
        declare_parameter("max_linear_mps", 0.5);
        declare_parameter("distance_tolerance_m", 0.02);
        // Read each value once, into a data member.
        target_distance_m_ =
            get_parameter("target_distance_m").as_double();
        kp_linear_ = get_parameter("kp_linear").as_double();
        max_linear_mps_ = get_parameter("max_linear_mps").as_double();
        distance_tolerance_m_ =
            get_parameter("distance_tolerance_m").as_double();
    }

  private:
    void odometry_callback(const nav_msgs::msg::Odometry &msg);
    void control_callback();
    void publish_command(double linear_speed);
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr
        odometry_subscription_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr
        command_publisher_;
    rclcpp::TimerBase::SharedPtr control_timer_;
    double target_distance_m_{0.0};    // how far to drive, in m
    double kp_linear_{0.0};            // gain: m/s per m of error
    double max_linear_mps_{0.0};       // the speed limit, in m/s
    double distance_tolerance_m_{0.0}; // stop within this, in m
    double start_x_{0.0};      // the start position, in m
    double start_y_{0.0};
    double x_{0.0};            // the position now, in m
    double y_{0.0};
    bool has_odometry_{false}; // true once odometry arrived
};

} // namespace l12_rosbot
