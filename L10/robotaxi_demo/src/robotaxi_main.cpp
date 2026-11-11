#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "robotaxi_demo/robotaxi_node.hpp"

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node{std::make_shared<robotaxi_demo::RoboTaxiNode>()};
    rclcpp::spin(node);
    RCLCPP_INFO(node->get_logger(), "exited the loop...");
    rclcpp::shutdown();
}
