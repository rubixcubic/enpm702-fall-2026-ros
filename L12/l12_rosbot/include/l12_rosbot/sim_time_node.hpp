#pragma once
#include <chrono>
#include <functional>
#include <rclcpp/rclcpp.hpp>

namespace l12_rosbot {

// Shows which clock drives a timer. Every 1 s, node_clock_callback
// runs on a timer of the node's clock (create_timer), and
// wall_clock_callback on a timer of the computer's clock
// (create_wall_timer). Each logs now(), the time of the node's
// clock. With use_sim_time true, the node's clock follows /clock.
class SimTimeNode : public rclcpp::Node {
  public:
    SimTimeNode()
        : rclcpp::Node{"sim_time_node"},
          node_clock_timer_{create_timer(
              std::chrono::seconds{1},
              std::bind(&SimTimeNode::node_clock_callback, this))},
          wall_clock_timer_{create_wall_timer(
              std::chrono::seconds{1},
              std::bind(&SimTimeNode::wall_clock_callback, this))} {}

  private:
    void node_clock_callback();
    void wall_clock_callback();
    rclcpp::TimerBase::SharedPtr node_clock_timer_;
    rclcpp::TimerBase::SharedPtr wall_clock_timer_;
};

} // namespace l12_rosbot
