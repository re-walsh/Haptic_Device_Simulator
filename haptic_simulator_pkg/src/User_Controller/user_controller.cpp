#include <chrono>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <std_msgs/msg/int32.hpp>
#include <eigen3/Eigen/Dense>

//TODO: Remove this namespace if neccessary
using namespace std::chrono_literals;

/*
Based on this minimal publisher/subscriber tutorial:
https://docs.ros.org/en/jazzy/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Cpp-Publisher-And-Subscriber.html
*/
#include "common.h"

class UserController : public rclcpp::Node
{
public:
  UserController()
  : Node("user_controller"), count_(0)
  {
    setpoint_publisher_ = this->create_publisher<std_msgs::msg::Float64MultiArray>("setpoint", 10);
    controller_type_publisher_ = this->create_publisher<std_msgs::msg::Int32>("controller_type", 10);
    auto timer_callback =
      [this]() -> void {
        auto message = std_msgs::msg::Float64MultiArray();
        Eigen::VectorXd vec = Eigen::VectorXd::Zero(6,1);
        message.data.assign(vec.data(), vec.data() + vec.size());
        // RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
        this->setpoint_publisher_->publish(message);

        auto type = std_msgs::msg::Int32();
        type.data = ControllerTypes::JointTorques;
        this->controller_type_publisher_->publish(type);
      };
    // Timer to create test data
    timer_ = this->create_wall_timer(500ms, timer_callback);
  }

private:
  // Topic Interaction TODO: Change the message types as appropriate
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr setpoint_publisher_;
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr controller_type_publisher_;
  // Timer
  rclcpp::TimerBase::SharedPtr timer_;
  size_t count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<UserController>());
  rclcpp::shutdown();
  return 0;
}