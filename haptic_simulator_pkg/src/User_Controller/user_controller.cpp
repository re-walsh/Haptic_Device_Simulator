#include <chrono>
#include <memory>
#include <string>
#include <iostream>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

using namespace std::chrono_literals;

/*
This Class is Based on this minimal publisher/subscriber tutorial:
https://docs.ros.org/en/jazzy/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Cpp-Publisher-And-Subscriber.html
*/

/**
 * A node to retrieve user data from the CLI, and publish it to the setpoint topic.
 * Works by periodically asking for a new joint position, and then publishing it.
 * (This node will ideally be replaced by a user's UI, or other program)
 */

class UserController : public rclcpp::Node
{
public:
  UserController()
  : Node("user_controller"), count_(0)
  {
    setpoint_publisher_ = this->create_publisher<std_msgs::msg::Float64MultiArray>("/arm_position_controller/commands", 10);
    auto timer_callback =
      [this]() -> void {
        // Get joint positon data from the user 
        float pos1, pos2, pos3, pos4, pos5, pos6;
        std::cout << "Please enter the desired joint position of each joint, seperated by whitespace." << std::endl;
        std::cin >> pos1 >> pos2 >> pos3 >> pos4 >> pos5 >> pos6;
        float data[6] = {pos1, pos2, pos3, pos4, pos5, pos6};

        // Put the data in a vector to be published to the other controllers.
        auto message = std_msgs::msg::Float64MultiArray();
        std::vector<double> vec(std::begin(data), std::end(data));
        message.data = vec;

        // Publish the data
        this->setpoint_publisher_->publish(message);
      };
    // Timer to periodically ask for input.
    timer_ = this->create_wall_timer(500ms, timer_callback);
  }

private:
  // Topic Interaction TODO: Change the message types as appropriate
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr setpoint_publisher_;
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