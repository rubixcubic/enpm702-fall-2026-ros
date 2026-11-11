# The oldest CMake that can build this package.
cmake_minimum_required(VERSION 3.8)
# The package's name: it must match <name> in package.xml.
project(robotaxi_demo)

# Compile as standard C++20, as in the C++ part of the course. Without these lines,
# g++ 13 uses its default, gnu++17: C++17 with GNU extensions.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # stop if the compiler has no C++20
set(CMAKE_CXX_EXTENSIONS OFF) # -std=c++20, not -std=gnu++20

# colcon build passes no build type. Debug adds -g, so breakpoints work in a debugger.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Turn on the compiler's warnings (GCC or Clang), as in the C++ part of the course:
# -Wall -Wextra: the common warnings; -pedantic-errors: code that is not standard C++
# is an error; -Wshadow: a variable that hides another one, such as a data member.
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -pedantic-errors -Wshadow)
endif()

# find dependencies: one line per package this package uses,
# the same packages as the <depend> lines of package.xml.
find_package(ament_cmake REQUIRED) # the build type of this package
find_package(rclcpp REQUIRED) # the ROS 2 C++ client library: nodes, publishers, timers
find_package(geometry_msgs REQUIRED) # the message type geometry_msgs/msg/Point

# Lets #include "robotaxi_demo/..." find the headers in include/.
include_directories(include)

# The publisher: builds the executable robotaxi_exe from its main and its node class.
# add_executable(robotaxi_exe src/robotaxi_main.cpp src/robotaxi_node.cpp)
add_executable(robotaxi_exe src/robotaxi_main.cpp src/robotaxi_node.cpp)
# Links robotaxi_exe to the ROS packages it uses.
ament_target_dependencies(robotaxi_exe rclcpp geometry_msgs)


# Copies robotaxi_exe to install/robotaxi_demo/lib/robotaxi_demo, where ros2 run looks for it.
install(TARGETS robotaxi_exe
  DESTINATION lib/${PROJECT_NAME})

# The subscriber: builds the executable dispatcher_exe the same way.
# add_executable(dispatcher_exe src/dispatcher_main.cpp src/dispatcher_node.cpp)
# ament_target_dependencies(dispatcher_exe rclcpp geometry_msgs)

# # Copies dispatcher_exe to install/robotaxi_demo/lib/robotaxi_demo, where ros2 run looks for it.
# install(TARGETS dispatcher_exe
#   DESTINATION lib/${PROJECT_NAME})

# Finishes the package (exports it so other packages can find it). It must be the last line.
ament_package()
