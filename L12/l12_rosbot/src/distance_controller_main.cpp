#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "l12_rosbot/distance_controller_node.hpp"

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node{std::make_shared<l12_rosbot::DistanceControllerNode>()};
    rclcpp::spin(node);
    if (rclcpp::ok()) {
        rclcpp::shutdown();
    }
}
