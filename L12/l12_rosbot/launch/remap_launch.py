from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    ld = LaunchDescription()
    # The heading controller in the namespace robot1, renamed
    # heading_a, with its two topics remapped to the ROSbot's.
    heading_controller = Node(
        package="l12_rosbot",
        executable="heading_controller_exe",
        name="heading_a",
        namespace="robot1",
        remappings=[
            ("odometry/filtered", "/odometry/filtered"),
            ("cmd_vel", "/cmd_vel"),
        ],
        parameters=[{"use_sim_time": True}],
        output="screen",
    )
    ld.add_action(heading_controller)
    return ld
