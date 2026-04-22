# Haptic Device Simulator
This repository is contains a testing environment built for the teleultrasound haptic device being developed as a capstone project for the mechanical engineering department at UBC.

The repository includes simulation and visualization functionality using Gazebo and Rviz, and is built on a Ros2 framework.

## Technical Requirements
- Ubuntu 22.04 "Jammy Jellyfish", Docker, or WSL
- Ros2 "Humble Hawksbill"
- Access to a haptic device controller library (this project assumes the user has access to the humanteleop/hapticdevice capstone repository)

## Getting Started
Ensure that you have Ubuntu 22.04 installed, either as an operating system, in WSL or in a docker container. If you do not have any of these, a docker container is recommended.

(Optional) Install Docker: https://docs.docker.com/get-started/get-docker/
1. Build the container: `docker compose -f Docker/docker-compose.yml build`
1. Start the container: `docker compose -f Docker/docker-compose.yml up`
1. Enter the container shell: `docker compose -f Docker/docker-compose.yml exec haptic_simulator bash`

Now, you can begin to set up the Ros2 environment and use the repository.
#### First time Setup
1. Install Ros2 Humble: https://docs.ros.org/en/humble/Installation.html 
1. Clone this repository in your Ros2 workspace using HTTPS: `git clone https://github.com/re-walsh/Haptic_Device_Simulator.git`
1. Load submodules (note this assumes you have access to the capstone repository): `git submodule update --init --recursive`
1. Install the required packages:
    ```
    sudo apt update 
    sudo apt install -y \
        ros-humble-ros-gz-sim \ 
        ros-humble-ros2-control \ 
        ros-humble-ros2-controllers \ 
        ros-humble-gz-ros2-control \
        ros-humble-joint-state-publisher-gui
1. Move to the per-use setup steps.
#### Per use setup 
1. Load ROS environment (once per terminal session): `source /opt/ros/humble/setup.bash`
1. Build: In root directory run `colcon build`
1. Load ROS package (in each terminal, for each new build): `source install/setup.bash`
1. Copy configuration files from HapticDevice/config to the executable directory: `cp -r haptic_comp/src/Capstone_Controller/hapticdevice/Code/HapticDevice/config/* install/haptic_comp/lib/haptic_comp/`
1. Run the user controller: `ros2 run haptic_comp user_controller`
1. (In a seperate terminal - don't forget to source!) Run the capstone controller: `ros2 run haptic_comp capstone_controller`
1. (In another seperate terminal) Run the simulator: `ros2 launch haptic_sim gazebo_ros2_control.launch.py`
1. (In one final terminal) Run the visualizer: `ros2 launch haptic_sim vis_launch.py manual:=false`

## Useful Comamnds
- `ros2 topic list` -> list topics
- `ros2 run rqt_graph rqt_graph` -> view a diagram of how topics and nodes are connected
- `tmux` -> a great terminal multiplexer, making it much easier to have multiple terminals open
- `colcon build` -> rebuild ros environment
- `colcon build --event-handlers console_direct+` - verbose build 
- `source /opt/ros/humble/setup.bash`-> run to allow ros to be used in terminal
- `source install/setup.bash` -> run first time a terminal is used, or when packages are updated



