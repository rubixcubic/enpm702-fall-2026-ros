#include "l12_rosbot/heading_controller_node.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "l12_rosbot/geometry.hpp"

namespace l12_rosbot {

namespace {
// Gains and limits, as constants in the code for now.
constexpr double target_yaw_deg{90.0};    // heading to reach, degrees
constexpr double kp_angular{1.5};         // gain: rad/s per rad
constexpr double max_angular_rps{1.0};    // speed limit, rad/s
constexpr double yaw_tolerance_rad{0.02}; // stop within this, rad
} // namespace

void HeadingControllerNode::odometry_callback(
    const nav_msgs::msg::Odometry &msg) {
    yaw_ = yaw_from_quaternion(msg.pose.pose.orientation);
    has_odometry_ = true;
}

void HeadingControllerNode::control_callback() {
    if (!has_odometry_) {
        return; // no heading yet
    }
    const double target_yaw{
        target_yaw_deg * std::numbers::pi / 180.0};
    // The error: the angle left to turn, the short way.
    // Positive: turn left.
    const double error{angle_difference(target_yaw, yaw_)};
    if (std::abs(error) < yaw_tolerance_rad) {
        publish_command(0.0);
        control_timer_->cancel(); // done: no more control_callback
        RCLCPP_INFO_STREAM(get_logger(),
                           "heading reached: yaw " << yaw_
                               << " rad, error " << error << " rad");
        return;
    }
    // The proportional control law, then the speed limit.
    const double command{
        std::clamp(kp_angular * error, -max_angular_rps,
                   max_angular_rps)};
    publish_command(command);
    RCLCPP_INFO_STREAM_THROTTLE(
        get_logger(), *get_clock(), 500,
        "error " << error << " rad, command " << command << " rad/s");
}

void HeadingControllerNode::publish_command(double angular_speed) {
    geometry_msgs::msg::TwistStamped command{};
    command.header.stamp = now();
    command.twist.angular.z = angular_speed;
    command_publisher_->publish(command);
}

} // namespace l12_rosbot
