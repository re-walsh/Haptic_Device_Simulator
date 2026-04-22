cmake_minimum_required(VERSION 3.8)

project(haptic_sim)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(simulator src/simulator.cpp)
ament_target_dependencies(simulator
  rclcpp
  std_msgs
  sensor_msgs
)

install(TARGETS
  simulator
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  config
  launch
  meshes
  scripts
  urdf
  visualization
  DESTINATION share/${PROJECT_NAME}
)

set(PACKAGE_DATA_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/building_robot.sdf"
)

install(FILES ${PACKAGE_DATA_FILES}
  DESTINATION share/${PROJECT_NAME}
)

install(PROGRAMS
  scripts/bringup_controllers.py
  scripts/validate_launch.py
  scripts/validate_command.py
  DESTINATION lib/${PROJECT_NAME}
)

# Copy config folder contents to install directory if it doesn't exist
install(DIRECTORY ../haptic_comp/Device_Controller/hapticdevice/Code/HapticDevice/config/
DESTINATION lib/${PROJECT_NAME})

ament_package()
