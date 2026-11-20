#include "l12_rosbot/heading_controller_node.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <string>

#include "l12_rosbot/geometry.hpp"

namespace l12_rosbot {

namespace {
// The ROSbot's own angular speed limit, in rad/s: max_velocity of
// angular.z in rosbot_controller/config/rosbot/controllers.yaml.
constexpr double rosbot_max_angular_rps{3.14};
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
    if (target_index_ >= target_headings_deg_.size()) {
        publish_command(0.0);
        control_timer_->cancel(); // done: no more control_callback
        RCLCPP_INFO_STREAM(get_logger(),
                           "all " << target_headings_deg_.size()
                                  << " headings reached");
        return;
    }
    const double target_deg{target_headings_deg_[target_index_]};
    const double target_yaw{target_deg * std::numbers::pi / 180.0};
    // The error: the angle left to turn, the short way.
    // Positive: turn left.
    const double error{angle_difference(target_yaw, yaw_)};
    if (std::abs(error) < yaw_tolerance_rad_) {
        publish_command(0.0);
        RCLCPP_INFO_STREAM(get_logger(),
                           "heading " << target_deg
                                      << " deg reached: yaw " << yaw_
                                      << " rad, error " << error
                                      << " rad");
        ++target_index_; // the next heading
        return;
    }
    // The proportional control law, then the speed limit.
    const double command{
        std::clamp(kp_angular_ * error, -max_angular_rps_,
                   max_angular_rps_)};
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

rcl_interfaces::msg::SetParametersResult
HeadingControllerNode::on_parameters_set(
    const std::vector<rclcpp::Parameter> &parameters) {
    rcl_interfaces::msg::SetParametersResult result{};
    // First check every new value. One bad value rejects the whole
    // change.
    for (const auto &parameter : parameters) {
        const std::string &name{parameter.get_name()};
        if (name == "target_headings_deg") {
            result.successful = false;
            result.reason =
                "target_headings_deg is read once, at start";
            return result;
        }
        if (name == "kp_angular" && parameter.as_double() < 0.0) {
            result.successful = false;
            result.reason = "kp_angular must not be negative";
            return result;
        }
        if (name == "max_angular_rps" &&
            (parameter.as_double() <= 0.0 ||
             parameter.as_double() > rosbot_max_angular_rps)) {
            result.successful = false;
            result.reason =
                "max_angular_rps must be more than 0 and at most "
                "3.14 rad/s";
            return result;
        }
    }
    // Every value is good: store the new ones, so the next
    // control_callback uses them.
    for (const auto &parameter : parameters) {
        const std::string &name{parameter.get_name()};
        if (name == "kp_angular") {
            kp_angular_ = parameter.as_double();
        } else if (name == "max_angular_rps") {
            max_angular_rps_ = parameter.as_double();
        } else if (name == "yaw_tolerance_rad") {
            yaw_tolerance_rad_ = parameter.as_double();
        }
        const std::string value{parameter.value_to_string()};
        RCLCPP_INFO_STREAM(get_logger(),
                           "new value: " << name << " = " << value);
    }
    result.successful = true;
    return result;
}

} // namespace l12_rosbot
