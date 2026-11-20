# The oldest CMake that can build this package.
cmake_minimum_required(VERSION 3.8)
# The package's name: it must match <name> in package.xml.
project(l12_rosbot)

# Compile as standard C++20, as in the C++ part of the course.
# Without these lines, g++ 13 uses its default, gnu++17: C++17 with
# GNU extensions.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # stop if the compiler has no C++20
set(CMAKE_CXX_EXTENSIONS OFF) # -std=c++20, not -std=gnu++20

# colcon build passes no build type. Debug adds -g, so breakpoints
# work in a debugger.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Turn on the compiler's warnings (GCC or Clang), as in the C++ part
# of the course: -Wall -Wextra: the common warnings; -pedantic-errors:
# code that is not standard C++ is an error; -Wshadow: a variable that
# hides another one, such as a data member.
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -pedantic-errors -Wshadow)
endif()

# find dependencies: one line per package this package uses,
# the same packages as the <depend> lines of package.xml.
find_package(ament_cmake REQUIRED) # this package's build type
# rclcpp: the ROS 2 C++ client library (nodes, publishers, timers)
find_package(rclcpp REQUIRED)
# geometry_msgs: the message types TwistStamped and Quaternion
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED) # the message type Odometry
# rcl_interfaces: the message type SetParametersResult
find_package(rcl_interfaces REQUIRED)

# Lets #include "l12_rosbot/..." find the headers in include/.
include_directories(include)

# Turns the ROSbot on the spot to a heading.
add_executable(heading_controller_exe
  src/heading_controller_main.cpp src/heading_controller_node.cpp)
ament_target_dependencies(heading_controller_exe
  rclcpp geometry_msgs nav_msgs rcl_interfaces)

# Drives the ROSbot straight ahead for a distance.
add_executable(distance_controller_exe
  src/distance_controller_main.cpp src/distance_controller_node.cpp)
ament_target_dependencies(distance_controller_exe
  rclcpp geometry_msgs nav_msgs)

# Logs now() from a timer of the node's clock and a wall timer.
add_executable(sim_time_exe
  src/sim_time_main.cpp src/sim_time_node.cpp)
ament_target_dependencies(sim_time_exe rclcpp)

# Copies the executables to install/l12_rosbot/lib/l12_rosbot, where
# ros2 run and ros2 launch look for them.
install(TARGETS
  heading_controller_exe distance_controller_exe sim_time_exe
  DESTINATION lib/${PROJECT_NAME})

# Copies the launch and config folders to
# install/l12_rosbot/share/l12_rosbot, where ros2 launch looks for
# launch files and FindPackageShare finds the YAML files.
install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME})

# Finishes the package (exports it so other packages can find it).
# It must be the last line.
ament_package()
