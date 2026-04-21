#include <array>
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

using namespace std::chrono_literals;

class Simulator : public rclcpp::Node
{
public:
  Simulator()
  : Node("simulator"),
    joint_names_({"joint1", "joint2", "joint3", "joint4", "joint5", "joint6"}),
    q_({0.0, 0.0, 0.0, 0.0, 0.0, 0.0}),
    qd_({0.0, 0.0, 0.0, 0.0, 0.0, 0.0}),
    tau_({0.0, 0.0, 0.0, 0.0, 0.0, 0.0}),
    inertia_({0.5, 0.5, 0.3, 0.2, 0.2, 0.1}),
    damping_({1.0, 1.0, 0.8, 0.6, 0.6, 0.4}),
    lower_limits_({-M_PI, -M_PI, 0.0, -M_PI, -M_PI, -M_PI}),
    upper_limits_({ M_PI,  M_PI, 0.1242, M_PI,  M_PI,  M_PI}),
    dt_(0.01)
  {
    torque_sub_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
      "/arm_position_controller/commands",
      10,
      std::bind(&Simulator::torque_callback, this, std::placeholders::_1));

    joint_pos_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
      "joint_pos", 10);

    joint_state_pub_ = this->create_publisher<sensor_msgs::msg::JointState>(
      "/joint_states", 10);

    timer_ = this->create_wall_timer(
      std::chrono::duration<double>(dt_),
      std::bind(&Simulator::sim_step, this));

    RCLCPP_INFO(
      this->get_logger(),
      "Simulator started. Interpreting /arm_position_controller/commands as torques.");
  }

private:
  void torque_callback(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
  {
    if (msg->data.size() != joint_names_.size()) {
      RCLCPP_WARN(
        this->get_logger(),
        "Expected %zu torques, got %zu. Ignoring.",
        joint_names_.size(),
        msg->data.size());
      return;
    }

    for (size_t i = 0; i < joint_names_.size(); ++i) {
      tau_[i] = msg->data[i];
    }
  }

  void sim_step()
  {
    for (size_t i = 0; i < joint_names_.size(); ++i) {
      const double qdd = (tau_[i] - damping_[i] * qd_[i]) / inertia_[i];

      qd_[i] += qdd * dt_;
      q_[i] += qd_[i] * dt_;

      if (q_[i] < lower_limits_[i]) {
        q_[i] = lower_limits_[i];
        qd_[i] = 0.0;
      } else if (q_[i] > upper_limits_[i]) {
        q_[i] = upper_limits_[i];
        qd_[i] = 0.0;
      }
    }

    publish_joint_pos();
    publish_joint_states();
  }

  void publish_joint_pos()
  {
    std_msgs::msg::Float64MultiArray msg;
    msg.data.assign(q_.begin(), q_.end());
    joint_pos_pub_->publish(msg);
  }

  void publish_joint_states()
  {
    sensor_msgs::msg::JointState msg;
    msg.header.stamp = this->get_clock()->now();
    msg.name.assign(joint_names_.begin(), joint_names_.end());
    msg.position.assign(q_.begin(), q_.end());
    msg.velocity.assign(qd_.begin(), qd_.end());
    msg.effort.assign(tau_.begin(), tau_.end());
    joint_state_pub_->publish(msg);
  }

  rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr torque_sub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr joint_pos_pub_;
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_pub_;
  rclcpp::TimerBase::SharedPtr timer_;

  const std::array<std::string, 6> joint_names_;
  std::array<double, 6> q_;
  std::array<double, 6> qd_;
  std::array<double, 6> tau_;
  const std::array<double, 6> inertia_;
  const std::array<double, 6> damping_;
  const std::array<double, 6> lower_limits_;
  const std::array<double, 6> upper_limits_;
  const double dt_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Simulator>());
  rclcpp::shutdown();
  return 0;
}
