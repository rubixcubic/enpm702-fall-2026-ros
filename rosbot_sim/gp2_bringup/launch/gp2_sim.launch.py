"""Gazebo Harmonic with the Husarion ROSbot in the GP2 patrol world.

Arguments: gui (true opens the Gazebo window, false runs the server only),
world (patrol, patrol_doors_swapped, empty, or a path to an .sdf file),
rviz (true opens RViz), x, y and yaw (the ROSbot's start pose), logical_camera (true
adds the front-facing logical camera; see the README).
"""

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, OpaqueFunction
from launch.launch_description_sources import AnyLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def logical_camera_enabled(context):
    return LaunchConfiguration("logical_camera").perform(context).lower() == "true"


def launch_setup(context):
    gui = LaunchConfiguration("gui").perform(context).lower() == "true"
    world = LaunchConfiguration("world").perform(context)
    if os.path.sep not in world:
        if not world.endswith(".sdf"):
            world += ".sdf"
        world = os.path.join(get_package_share_directory("gp2_bringup"), "worlds", world)
    if not os.path.isfile(world):
        raise RuntimeError(f"world file not found: {world}")

    simulation = IncludeLaunchDescription(
        AnyLaunchDescriptionSource(
            os.path.join(get_package_share_directory("rosbot_gazebo"), "launch",
                         "simulation.yaml")),
        launch_arguments={
            "gz_world": world,
            "gz_headless_mode": "False" if gui else "True",
            # The Gazebo window: Husarion's layout (husarion_gz_worlds/config/teleop.config) with a camera pose that
            # shows the whole patrol world.
            "gz_gui": os.path.join(get_package_share_directory("gp2_bringup"), "config", "gp2_gui.config"),
            "robot_model": "rosbot",
            "rviz": "True" if LaunchConfiguration("rviz").perform(context).lower() == "true" else "False",
            "x": LaunchConfiguration("x").perform(context),
            "y": LaunchConfiguration("y").perform(context),
            "yaw": LaunchConfiguration("yaw").perform(context),
        }.items(),
    )

    # The fixed camera of the world (the worlds of this package hold one).
    bridge_arguments = [
        "patrol_cam/image@sensor_msgs/msg/Image[gz.msgs.Image",
        "patrol_cam/camera_info@sensor_msgs/msg/CameraInfo[gz.msgs.CameraInfo",
    ]
    actions = [simulation]

    if logical_camera_enabled(context):
        # The logical camera: a small model with the sensor, spawned at the ROSbot's start
        # pose and joined to the ROSbot's base_link by a fixed joint (see the SDF file).
        # rosbot_sim stays as it is.
        rig = os.path.join(get_package_share_directory("gp2_bringup"), "models",
                           "logical_camera_rig.sdf")
        spawn_rig = Node(
            package="ros_gz_sim",
            executable="create",
            name="spawn_logical_camera_rig",
            arguments=["-file", rig, "-name", "logical_camera_rig",
                       "-x", LaunchConfiguration("x").perform(context),
                       "-y", LaunchConfiguration("y").perform(context),
                       "-z", "0.0",
                       "-Y", LaunchConfiguration("yaw").perform(context)],
            output="screen",
        )
        # TF: the camera frame, 0.12 m ahead of and 0.20 m above base_link (as in the SDF).
        camera_frame = Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            name="logical_camera_frame",
            arguments=["--x", "0.12", "--y", "0.0", "--z", "0.20",
                       "--frame-id", "base_link", "--child-frame-id", "logical_camera_link"],
            output="screen",
        )
        bridge_arguments.append(
            "logical_camera@ros_gz_interfaces/msg/LogicalCameraImage"
            "[gz.msgs.LogicalCameraImage")
        actions += [spawn_rig, camera_frame]

    actions.append(Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        name="patrol_cam_bridge",
        arguments=bridge_arguments,
        parameters=[{"use_sim_time": True}],
        output="screen",
    ))
    return actions


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument("gui", default_value="true", choices=["true", "false"],
                              description="Open the Gazebo window (false: server only)."),
        DeclareLaunchArgument("world", default_value="patrol",
                              description="patrol, patrol_doors_swapped, empty, or a path to an .sdf file."),
        DeclareLaunchArgument("rviz", default_value="false", choices=["true", "false"],
                              description="Open RViz."),
        DeclareLaunchArgument("logical_camera", default_value="true", choices=["true", "false"],
                              description="Add the front-facing logical camera."),
        DeclareLaunchArgument("x", default_value="0.0", description="Start x in meters."),
        DeclareLaunchArgument("y", default_value="0.0", description="Start y in meters."),
        DeclareLaunchArgument("yaw", default_value="0.0", description="Start heading in radians."),
        OpaqueFunction(function=launch_setup),
    ])
