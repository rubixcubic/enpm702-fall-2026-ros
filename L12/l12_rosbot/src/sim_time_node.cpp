#include "l12_rosbot/sim_time_node.hpp"

#include <iomanip>

namespace l12_rosbot {

void SimTimeNode::node_clock_callback() {
    RCLCPP_INFO_STREAM(get_logger(),
                       "create_timer:      now() = "
                           << std::fixed << std::setprecision(3)
                           << now().seconds() << " s");
}

void SimTimeNode::wall_clock_callback() {
    RCLCPP_INFO_STREAM(get_logger(),
                       "create_wall_timer: now() = "
                           << std::fixed << std::setprecision(3)
                           << now().seconds() << " s");
}

} // namespace l12_rosbot
