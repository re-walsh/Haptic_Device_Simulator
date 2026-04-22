#include <memory>
#include <chrono>
#include <string>
#include <sstream>

#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/float64_multi_array.hpp>
#include <sensor_msgs/msg/joint_state.hpp>

#include "haptic_device.h"

using namespace HapticDevice;
using namespace std::chrono_literals;

template<typename T>
class CapstoneControllerBase : public Controller<T>, public rclcpp::Node {
public:
    CapstoneControllerBase(const std::string& name = "controller") : rclcpp::Node(name) {
        // Setpoint subscription ( <- from user controller )
        auto setpoint_callback =
            [this](std_msgs::msg::Float64MultiArray::UniquePtr msg) -> void {
                Eigen::VectorXd vec = Eigen::Map<const Eigen::VectorXd>(msg->data.data(), msg->data.size());
                ErrorCode error = this->set_target_state(vec);
                if (error != ErrorCode::Success){
                    RCLCPP_ERROR(this->get_logger(), "Error setting target state. (Likely the incorrect vector size)");
                }
                std::stringstream ss;
                ss << "Setpoint received: " << vec.transpose();
                std::string str = ss.str();
                RCLCPP_INFO(this->get_logger(), str.c_str());
            };
        this->setpoint_subscription_ = this->create_subscription<std_msgs::msg::Float64MultiArray>("setpoint", 10, setpoint_callback);

        // Torque publisher ( -> to simulation )
        auto joint_torque_callback = 
            [this]() -> void {
                auto message = std_msgs::msg::Float64MultiArray();
                auto& torque_data = this->robot_state.JointTorques.data;
                message.data.assign(torque_data.data(), torque_data.data() + torque_data.size()); 
                this->joint_torque_publisher_->publish(message);
            };
        this->joint_torque_publisher_ = 
            this->create_publisher<std_msgs::msg::Float64MultiArray>("arm_position_controller/commands", 10);
        timer_ = this->create_wall_timer(500ms, joint_torque_callback);

        // Position subscription ( <- from simulation )
        auto joint_pos_callback =
            [this](sensor_msgs::msg::JointState::UniquePtr msg) -> void {
                Eigen::VectorXd vec = Eigen::Map<const Eigen::VectorXd>(msg->position.data(), msg->position.size() - 1);   // dont include joint3b_mimic
                if(this->robot_state.JointPositions.data.size() != vec.size()){
                    RCLCPP_ERROR(this->get_logger(), "Incorrect joint position vector size (%ld).", vec.size());
                }
                this->robot_state.JointPositions.data = vec;
            };
        this->joint_pos_subscription_ =
            this->create_subscription<sensor_msgs::msg::JointState>("joint_states", 10, joint_pos_callback);
    }
    
private:
    T _target_state;
    
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr setpoint_subscription_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_pos_subscription_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr joint_torque_publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

 
    ErrorCode set_target_state(Eigen::VectorXd setpoint) {
        if(this->_target_state.data.size() == setpoint.size()){
            this->_target_state.data = setpoint;
            return ErrorCode::Success;
        }
        return ErrorCode::IncompleteData;
    }

    T get_target_state() override {
        return _target_state;
    }

};

// define specific class types
// template class CapstoneControllerBase<HapticDevice::JointTauVector>;
// template class CapstoneControllerBase<HapticDevice::JointQPositionVector>;
// template class CapstoneControllerBase<HapticDevice::Wrench>;
// template class CapstoneControllerBase<HapticDevice::CartesianPose>;