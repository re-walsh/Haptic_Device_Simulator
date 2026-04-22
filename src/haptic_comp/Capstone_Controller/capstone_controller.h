#include <memory>
#include <chrono>
#include <string>
#include <sstream>

#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/float64_multi_array.hpp>

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
                RCLCPP_ERROR(this->get_logger(), "Error setting target state. Likely the incorrect vector size.");
            }
            std::stringstream ss;
            ss << "Setpoint received: " << vec.transpose();
            std::string str = ss.str();
            RCLCPP_INFO(this->get_logger(), str.c_str());
        };
        this->setpoint_subscription_ = this->create_subscription<std_msgs::msg::Float64MultiArray>("setpoint", 10, setpoint_callback);

        // Torque publisher ( -> to simulation )


        // Position subscription ( <- from simulation )



        
        // TODO: maybe incorporate into run() from HapticDevice::Controller<T>
        // Publish joint torque data
        // joint_torque_publisher_ = this->create_publisher<std_msgs::msg::String>("joint_torque", 10);


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