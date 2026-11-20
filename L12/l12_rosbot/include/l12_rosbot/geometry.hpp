#pragma once
#include <cmath>
#include <geometry_msgs/msg/quaternion.hpp>
#include <numbers>

namespace l12_rosbot {

/**
 * @brief Heading (yaw) in radians of a quaternion, counterclockwise
 * from +x.
 * @param orientation The orientation, for example
 * `odometry.pose.pose.orientation`.
 * @return The yaw, in (-pi, pi].
 */
inline double yaw_from_quaternion(
    const geometry_msgs::msg::Quaternion &orientation) {
    const double siny_cosp{
        2.0 * (orientation.w * orientation.z +
               orientation.x * orientation.y)};
    const double cosy_cosp{
        1.0 - 2.0 * (orientation.y * orientation.y +
                     orientation.z * orientation.z)};
    return std::atan2(siny_cosp, cosy_cosp);
}

/**
 * @brief Wrap an angle to the range (-pi, pi].
 * @param angle An angle in radians, of any size.
 * @return The same direction as an angle greater than -pi and at
 * most pi.
 */
inline double wrap_angle(double angle) {
    constexpr double two_pi{2.0 * std::numbers::pi};
    double wrapped{std::fmod(angle + std::numbers::pi, two_pi)};
    if (wrapped <= 0.0) {
        wrapped += two_pi;
    }
    return wrapped - std::numbers::pi;
}

/**
 * @brief Signed angle to turn from one heading to another, the short
 * way.
 * @param target The heading to reach, in radians.
 * @param current The heading now, in radians.
 * @return target - current, wrapped to (-pi, pi]. Positive means turn
 * counterclockwise.
 */
inline double angle_difference(double target, double current) {
    return wrap_angle(target - current);
}

} // namespace l12_rosbot
