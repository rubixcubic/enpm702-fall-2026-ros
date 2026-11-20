#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "l12_rosbot/sim_time_node.hpp"

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node{std::make_shared<l12_rosbot::SimTimeNode>()};
    rclcpp::spin(node);
    if (rclcpp::ok()) {
        rclcpp::shutdown();
    }
}
