from ast import arguments
from http.server import executable
from pathlib import Path
from re import M
import shutil
import tempfile
from urllib import robotparser

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, LogInfo, OpaqueFunction, RegisterEventHandler, SetEnvironmentVariable
from launch.substitutions import LaunchConfiguration
from launch.conditions import IfCondition
from launch_ros.actions import Node

def generate_launch_description():
    pkg_share = Path(get_package_share_directory('haptic_sim'))
    
    rviz_config = pkg_share / 'visualization' / 'view_haptic.rviz'
    urdf_template = pkg_share / 'urdf' / 'hapticdevice_URDF.urdf'
    with open(urdf_template, 'r') as infp:
        robot_description_config = infp.read()

    manual = LaunchConfiguration('manual')
    
    

    node_rviz = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d',str(rviz_config)],
        parameters=[{'use_sim_time': True}]
    )
    node_robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output ='screen',
        parameters=[{'robot_description':robot_description_config}]
    )
    node_joint_state_gui = Node(
        package='joint_state_publisher_gui',
        executable='joint_state_publisher_gui',
        name='joint_state_publisher_gui',
        output='screen',
        condition=IfCondition(manual)
    )
    node_plot = Node(
        package='rqt_plot',
        executable='rqt_plot',
        name='joint_plot',
        
        arguments=['/joint_states/position[0]', 
                   '/joint_states/position[1]', 
                   '/joint_states/position[2]',
                   '/joint_states/position[3]',
                   '/joint_states/position[4]',
                   '/joint_states/position[5]',
                   '/joint_states/position[6]'] 
    )
   

    return LaunchDescription([
        DeclareLaunchArgument(
            'manual',
            default_value='true',
            description='Run RViz2 without Gazebo sim.',
        ),
        node_robot_state_publisher,
        node_rviz,
        node_joint_state_gui,
        node_plot,
       
    ])

