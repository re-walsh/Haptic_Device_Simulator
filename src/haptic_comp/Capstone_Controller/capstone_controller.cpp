#include <memory>
#include <chrono>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "haptic_device.h"

/*
Based on this minimal publisher/subscriber tutorial:
https://docs.ros.org/en/jazzy/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Cpp-Publisher-And-Subscriber.html

This is where Edwin + Kyle's capstone functionality can be implemented.

NOTE: In this implementation, raw joint poisition and joint position are assumed to be the same topic, so
the capstone controller only subscribes to the topic, and does not publish any joint position data.
*/
//TODO: Remove this namespace if neccessary
using namespace std::chrono_literals;
using namespace HapticDevice;

class ExampleUserController : public Controller<JointTauVector> {

public:

  JointTauVector torques;
  int direction = 1;

	ExampleUserController() {
    torques.data = Eigen::MatrixXd::Zero(6, 1);
  }

	HapticDevice::ErrorCode start() override {
    ErrorCode error = Controller<JointTauVector>::start();
	  if (error != ErrorCode::Success) return error;

    // custom user stuff

    return ErrorCode::Success;
  };

	HapticDevice::JointTauVector get_target_state() override {
    if(this->get_robot_state().JointTorques.data(0) >= this->limits.config["joints"][0]["torque_max"].as<double>(10.0))
    {
      direction = -1 * direction;
    }
    if(this->get_robot_state().JointTorques.data(0) <= this->limits.config["joints"][0]["torque_min"].as<double>(10.0))
    {
      direction = -1 * direction;
    }
    
    this->torques.data(0) += direction;
    return torques;
  };
};

class CapstoneController : public rclcpp::Node
{
public:
  CapstoneController()
  : Node("capstone_controller")
  {
    // Setpoint data retrieval
    auto setpoint_callback =
      [this](std_msgs::msg::Float64MultiArray::UniquePtr msg) -> void {
        this->setpoints = msg->data;
        if(!(this->setpoints.empty())){
          RCLCPP_INFO(this->get_logger(), "Processing setpoint data: %f, %f, %f, %f, %f, %f", 
          this->setpoints.at(0), this->setpoints.at(1), this->setpoints.at(2), this->setpoints.at(3), this->setpoints.at(4), this->setpoints.at(5));
        }
      };
    setpoint_subscription_ =
      this->create_subscription<std_msgs::msg::Float64MultiArray>("setpoint", 10, setpoint_callback);
    
    // Joint position data retrieval
    auto joint_pos_callback =
      [this](std_msgs::msg::Float64MultiArray::UniquePtr msg) -> void {
        RCLCPP_INFO(this->get_logger(), "I heard joint_pos info: %f, %f, %f, %f, %f, %f", msg->data.at(0),msg->data.at(1),msg->data.at(2),msg->data.at(3),msg->data.at(4),msg->data.at(5));
        this->positions = msg->data;
      };
    joint_pos_subscription_ =
      this->create_subscription<std_msgs::msg::Float64MultiArray>("joint_pos", 10, joint_pos_callback);

    // Publish joint torque data
    joint_torque_publisher_ = 
      this->create_publisher<std_msgs::msg::Float64MultiArray>("arm_position_controller/commands", 10);

    // Periodically update the simulator.
    auto timer_callback =
      [this]() -> void {
        auto message = std_msgs::msg::Float64MultiArray();
        // TODO: Calculate actual torques using the capstone library
        if(!(this->setpoints.empty()))
          message.data = this->setpoints;
        //RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
        this->joint_torque_publisher_->publish(message);
      
      };
    
    // Timer to create test data
    timer_ = this->create_wall_timer(500ms, timer_callback);

    // start controller
    this->controller.start();
    this->controller.task_debug_hud.stop();
   // this->controller.run_task.set
  }

private:
  // User developed controller
  ExampleUserController controller;
  // User specificed joint angles
  std::vector<double> setpoints;
  // Actual joint positions from the simulator
  std::vector<double> positions;

  // Topic interaction TODO: Change the message types as appropriate
  rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr setpoint_subscription_;
  rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr joint_pos_subscription_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr joint_torque_publisher_;

  // Timer
  rclcpp::TimerBase::SharedPtr timer_;
  size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CapstoneController>());
  rclcpp::shutdown();
  return 0;
}