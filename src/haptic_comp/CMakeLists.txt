cmake_minimum_required(VERSION 3.8)
project(haptic_comp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(Eigen3 REQUIRED)

# Add Node executables
# User Controller
add_executable(node_user_controller User_Controller/user_controller.cpp)
ament_target_dependencies(node_user_controller rclcpp std_msgs)
ament_target_dependencies(node_user_controller rclcpp std_msgs Eigen3)
target_include_directories(node_user_controller PRIVATE include)

# Device Controller
add_executable(node_device_controller Device_Controller/controller_manager.cpp)
ament_target_dependencies(node_device_controller rclcpp std_msgs)
target_include_directories(node_device_controller PRIVATE include)
ament_target_dependencies(node_device_controller sensor_msgs)

# add HapticDevice library
set(HW_ENABLE OFF CACHE BOOL "" FORCE)  # disable hardware for now until full linux hardware compatibility is available.
add_subdirectory(Device_Controller/hapticdevice/Code/HapticDevice/)
target_link_libraries(node_device_controller HapticDevice)

install(TARGETS
  node_user_controller
  node_device_controller
  DESTINATION lib/${PROJECT_NAME})

# Install HapticDevice library
install(TARGETS HapticDevice
  LIBRARY DESTINATION lib/
  RUNTIME DESTINATION lib/)

# Copy config folder contents to install directory if it doesn't exist
install(DIRECTORY Device_Controller/hapticdevice/Code/HapticDevice/config/
DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
