#include "l12_rosbot/distance_controller_node.hpp"

#include <algorithm>
#include <cmath>

namespace l12_rosbot {

namespace {
// Gains and limits, as constants in the code for now.
constexpr double target_distance_m{1.0};     // how far to drive, m
constexpr double kp_linear{0.8};             // gain: m/s per m
constexpr double max_linear_mps{0.5};        // speed limit, m/s
constexpr double distance_tolerance_m{0.02}; // stop within this, m
} // namespace

void DistanceControllerNode::odometry_callback(
    const nav_msgs::msg::Odometry &msg) {
    x_ = msg.pose.pose.position.x;
    y_ = msg.pose.pose.position.y;
    if (!has_odometry_) {
        start_x_ = x_; // the first message gives the start
        start_y_ = y_;
        has_odometry_ = true;
    }
}

void DistanceControllerNode::control_callback() {
    if (!has_odometry_) {
        return; // no position yet
    }
    const double driven{std::hypot(x_ - start_x_, y_ - start_y_)};
    // The error: the distance left to drive.
    const double error{target_distance_m - driven};
    if (std::abs(error) < distance_tolerance_m) {
        publish_command(0.0);
        control_timer_->cancel(); // done: no more control_callback
        RCLCPP_INFO_STREAM(get_logger(),
                           "distance reached: driven " << driven
                               << " m, error " << error << " m");
        return;
    }
    // The proportional control law, then the speed limit.
    const double command{
        std::clamp(kp_linear * error, -max_linear_mps,
                   max_linear_mps)};
    publish_command(command);
    RCLCPP_INFO_STREAM_THROTTLE(
        get_logger(), *get_clock(), 500,
        "error " << error << " m, command " << command << " m/s");
}

void DistanceControllerNode::publish_command(double linear_speed) {
    geometry_msgs::msg::TwistStamped command{};
    command.header.stamp = now();
    command.twist.linear.x = linear_speed;
    command_publisher_->publish(command);
}

} // namespace l12_rosbot
