#pragma once
#include <chrono>
#include <cstddef>
#include <functional>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <rcl_interfaces/msg/set_parameters_result.hpp>
#include <rclcpp/rclcpp.hpp>
#include <vector>

namespace l12_rosbot {

// Turns the ROSbot on the spot to each heading of a list, in order,
// with a proportional controller. odometry_callback stores the
// robot's heading. Every 50 ms (20 Hz), control_callback computes the
// error, the angle left to turn, and publishes an angular speed
// proportional to it. Within the tolerance, it moves on to the next
// heading, and stops after the last one. The headings, the gain, the
// speed limit and the tolerance are parameters.
// on_parameters_set checks a new gain or speed limit, then stores the
// new gain, speed limit or tolerance, so a change takes effect while
// the robot turns.
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
                        this))} {
        // Declare each parameter with its default value.
        declare_parameter(
            "target_headings_deg",
            std::vector<double>{90.0, 180.0, -90.0, 0.0});
        declare_parameter("kp_angular", 1.5);
        declare_parameter("max_angular_rps", 1.0);
        declare_parameter("yaw_tolerance_rad", 0.02);
        // Read each value once, into a data member.
        target_headings_deg_ =
            get_parameter("target_headings_deg").as_double_array();
        kp_angular_ = get_parameter("kp_angular").as_double();
        max_angular_rps_ =
            get_parameter("max_angular_rps").as_double();
        yaw_tolerance_rad_ =
            get_parameter("yaw_tolerance_rad").as_double();
        // Runs each time someone sets one of this node's parameters.
        parameter_callback_handle_ = add_on_set_parameters_callback(
            std::bind(&HeadingControllerNode::on_parameters_set, this,
                      std::placeholders::_1));
    }

  private:
    void odometry_callback(const nav_msgs::msg::Odometry &msg);
    void control_callback();
    void publish_command(double angular_speed);
    rcl_interfaces::msg::SetParametersResult on_parameters_set(
        const std::vector<rclcpp::Parameter> &parameters);
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr
        odometry_subscription_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr
        command_publisher_;
    rclcpp::TimerBase::SharedPtr control_timer_;
    // Keeps on_parameters_set registered: the callback is removed
    // when the handle is destroyed.
    rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr
        parameter_callback_handle_;
    std::vector<double> target_headings_deg_; // in degrees, in order
    double kp_angular_{0.0};        // gain: rad/s per rad of error
    double max_angular_rps_{0.0};   // the speed limit, in rad/s
    double yaw_tolerance_rad_{0.0}; // stop within this, in rad
    std::size_t target_index_{0};   // which heading it turns to now
    double yaw_{0.0};               // the robot's heading, in rad
    bool has_odometry_{false};      // true once odometry arrived
};

} // namespace l12_rosbot
