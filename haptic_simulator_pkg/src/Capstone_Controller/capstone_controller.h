#include <memory>
#include <chrono>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/float64_multi_array.hpp>

#include "haptic_device.h"

using namespace HapticDevice;

template<typename T>
class ROSControllerBase : public Controller<T>, public rclcpp::Node {
    public:
    ROSControllerBase() : rclcpp::Node("capstone_controller") {
        // Setpoint data retrieval
        auto setpoint_callback =
        [this](std_msgs::msg::Float64MultiArray::UniquePtr msg) -> void {
            Eigen::VectorXd vec = Eigen::Map<const Eigen::VectorXd>(msg->data.data(), msg->data.size());
            ErrorCode error = this->set_target_state(vec);
            if (error != ErrorCode::Success){
                RCLCPP_ERROR(this->get_logger(), "Error setting target state. Likely the incorrect vector size.");
            }
            // RCLCPP_INFO(this->get_logger(), "I heard setpoint info: '%s'", msg->data.c_str());
        };
        this->setpoint_subscription_ = this->create_subscription<std_msgs::msg::Float64MultiArray>("setpoint", 10, setpoint_callback);
        
        // TODO: maybe incorporate into run() from HapticDevice::Controller<T>
        // // Publish joint torque data
        // joint_torque_publisher_ = this->create_publisher<std_msgs::msg::String>("joint_torque", 10);
        // auto timer_callback =
        // [this]() -> void {
        //     auto message = std_msgs::msg::Float64MultiArray();
        //     message.data = "Fake joint_torque data from capstone: " + std::to_string(this->controller->get_robot_state().JointTorques.data(0));
        //     RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
        //     this->joint_torque_publisher_->publish(message);
        // };
        // // Timer to create test data
        // timer_ = this->create_wall_timer(500ms, timer_callback);


        // TODO: maybe override update_robot_state() ?
        // // Joint position data retrieval
        // auto joint_pos_callback =
        // [this](std_msgs::msg::String::UniquePtr msg) -> void {
        //     RCLCPP_INFO(this->get_logger(), "I heard joint_pos info: '%s'", msg->data.c_str());
        // };
        // joint_pos_subscription_ =
        // this->create_subscription<std_msgs::msg::String>("joint_pos", 10, joint_pos_callback);
    }
    
    private:
    T _target_state;
    
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr setpoint_subscription_;
    // rclcpp::Publisher<std_msgs::msg::String>::SharedPtr joint_torque_publisher_;
    // Timer
    rclcpp::TimerBase::SharedPtr timer_;
    size_t count_;
    
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
template class ROSControllerBase<HapticDevice::JointTauVector>;
template class ROSControllerBase<HapticDevice::JointQPositionVector>;
template class ROSControllerBase<HapticDevice::Wrench>;
template class ROSControllerBase<HapticDevice::CartesianPose>;
