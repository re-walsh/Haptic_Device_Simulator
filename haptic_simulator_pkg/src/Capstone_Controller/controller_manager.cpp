
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

#include "capstone_controller.h"
#include "common.h"

class ControllerManager : public rclcpp::Node {
public:
    ControllerManager() : rclcpp::Node("controller_manager") {
        // Controller type modifier
        auto controller_type_callback =
        [this](std_msgs::msg::Int32::UniquePtr msg) -> void {
            if (this->controller_type != msg->data){    // if different, change controllers
                RCLCPP_INFO(this->get_logger(), "CHANGING CONTROLLER %d -> %d", this->controller_type, msg->data);
                this->change_controller(msg->data);
            }
        };
        controller_type_subscription_ = this->create_subscription<std_msgs::msg::Int32>("controller_type", 10, controller_type_callback);

        auto timer_callback =
        [this]() -> void {
            std::stringstream ss;
            // ss << "\033[H\033[J";
            ss << "Joint Torques: " << this->active_controller->get_robot_state().JointTorques.data.transpose();
            ss << "\tJoint Positions: " << this->active_controller->get_robot_state().JointPositions.data.transpose();
            ss << "\tController Type: " << this->controller_type;
            std::string str = ss.str();
            RCLCPP_INFO(this->get_logger(), str.c_str());
        };
        // Timer to create test data
        timer_ = this->create_wall_timer(500ms, timer_callback);
    }

private:
    std::unique_ptr<HapticDevice::ControllerInterface> active_controller;
    int controller_type = ControllerTypes::COUNT;
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr controller_type_subscription_;
    // Timer
    rclcpp::TimerBase::SharedPtr timer_;
    size_t count_;

    void change_controller(int type){
        // stop the old controller
        if(this->active_controller){
            this->active_controller->stop();
            this->active_controller.reset();
        }
        
        switch(type){
            case ControllerTypes::JointTorques:
            this->active_controller = std::make_unique<CapstoneControllerBase<HapticDevice::JointTauVector>>("joint_torq_controller");
            break;
            
            case ControllerTypes::JointPosition:
            this->active_controller = std::make_unique<CapstoneControllerBase<HapticDevice::JointQPositionVector>>("joint_pos_controller");
            break;
            
            case ControllerTypes::Wrench:
            this->active_controller = std::make_unique<CapstoneControllerBase<HapticDevice::Wrench>>("wrench_controller");
            break;
            
            case ControllerTypes::CartesianPose:
            this->active_controller = std::make_unique<CapstoneControllerBase<HapticDevice::CartesianPose>>("pose_controller");
            break;

            default:
            RCLCPP_ERROR(this->get_logger(), "Error invalid controller type: %d", type);
            // handle invalid type selection
        }
        
        // start the new controller
        if(this->active_controller){
            this->active_controller->start();
        }
        
        this->controller_type = type;
    }
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ControllerManager>());
    rclcpp::shutdown();
    return 0;
}