# General Overview

## Technical Requirements
- Ubuntu 24.04
- Ros2 "Jazzy Jalisco"
- Docker (optional) 

## Useful commands
- `colcon build` -> rebuild ros environment
- `colcon build --event-handlers console_direct+` - verbose build 
- `source /opt/ros/jazzy/setup.bash`-> run to allow ros to be used in terminal
- `source install/setup.bash` -> run first time a terminal is used, or when packages are updated
- `ros2 topic list` -> list topics

## Repo Setup
1. Load submodules: `git submodule update --init --recursive`

## Docker Setup
Install Docker: https://docs.docker.com/get-started/get-docker/
1. Build the container: `docker compose -f Docker/docker-compose.yml build`
1. Start the container: `docker compose -f Docker/docker-compose.yml up`
1. Enter the container shell: `docker compose -f Docker/docker-compose.yml exec haptic_simulator bash`

## Run ROS inside Docker Container
1. Load ROS environment (once per terminal session): `source /opt/ros/jazzy/setup.bash`
1. Build: In root directory (haptic_simulator_pkg) run `colcon build`
1. Load ROS package (for each new build): `source install/setup.bash`
1. Copy configuration files from HapticDevice/config to the executable directory: `cp -r src/Capstone_Controller/hapticdevice/Code/HapticDevice/config/* install/haptic_sim_pkg/lib/haptic_sim_pkg/`
1. Run program: `ros2 run haptic_sim_pkg capstone_controller`

## How to test basic system
In top lvl workspace:
- `colcon build`

In 4 different top lvl workspace terminals:
- `source /opt/ros/jazzy/setup.bash`
- `source install/setup.bash`

Now run 1 module in each of those terminals
- `ros2 run haptic_sim_pkg capstone_controller`
- `ros2 run haptic_sim_pkg user_controller`
- `ros2 run haptic_sim_pkg simulator`
- `ros2 run haptic_sim_pkg visualization`


Visual summary:
![Visual Summary](images/overview.png)
