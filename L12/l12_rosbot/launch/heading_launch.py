from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    ld = LaunchDescription()
    # The YAML file to load: config/controllers.yaml, unless the
    # command names another one.
    params_file = DeclareLaunchArgument(
        "params_file",
        default_value=PathJoinSubstitution(
            [FindPackageShare("l12_rosbot"), "config",
             "controllers.yaml"]
        ),
        description="YAML file with the controller's parameters",
    )
    heading_controller = Node(
        package="l12_rosbot",
        executable="heading_controller_exe",
        parameters=[LaunchConfiguration("params_file")],
        output="screen",
    )
    ld.add_action(params_file)
    ld.add_action(heading_controller)
    return ld
