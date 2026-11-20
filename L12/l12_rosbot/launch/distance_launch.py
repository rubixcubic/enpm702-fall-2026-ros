from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    ld = LaunchDescription()
    # The parameters are written in the launch file, as a dictionary.
    distance_controller = Node(
        package="l12_rosbot",
        executable="distance_controller_exe",
        parameters=[
            {
                "use_sim_time": True,
                "target_distance_m": 1.0,
                "kp_linear": 0.8,
                "max_linear_mps": 0.5,
                "distance_tolerance_m": 0.02,
            }
        ],
        output="screen",
    )
    ld.add_action(distance_controller)
    return ld
