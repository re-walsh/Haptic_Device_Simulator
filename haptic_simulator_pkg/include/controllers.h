#include "haptic_device.h"

using namespace HapticDevice;


class JointTorqueController : public Controller<JointTauVector> {
public:
	JointTauVector get_target_state() override {
        return this->setpoint;
    };
    ErrorCode set_target_state(Eigen::VectorXd setpoint) {
        if(this->setpoint.data.size() == setpoint.size()){
            this->setpoint.data = setpoint;
            return ErrorCode::Success;
        }
        return ErrorCode::IncompleteData;
    }
    private:
    JointTauVector setpoint;
};

class JointPositionController : public Controller<JointQPositionVector> {
public:
    JointQPositionVector get_target_state() override {
        return this->setpoint;
    };
    ErrorCode set_target_state(Eigen::VectorXd setpoint) {
        if(this->setpoint.data.size() == setpoint.size()){
            this->setpoint.data = setpoint;
            return ErrorCode::Success;
        }
        return ErrorCode::IncompleteData;
    }
    private:
    JointQPositionVector setpoint;
};

class WrenchController : public Controller<Wrench> {
public:
    Wrench get_target_state() override {
        return this->setpoint;
    };
    ErrorCode set_target_state(Eigen::VectorXd setpoint) {
        if(this->setpoint.data.size() == setpoint.size()){
            this->setpoint.data = setpoint;
            return ErrorCode::Success;
        }
        return ErrorCode::IncompleteData;
    }
    private:
    Wrench setpoint;
};

class CartesianPoseController : public Controller<CartesianPose> {
public:
    CartesianPose get_target_state() override {
        return this->setpoint;
    };
    ErrorCode set_target_state(Eigen::VectorXd setpoint) {
        if(this->setpoint.data.size() == setpoint.size()){
            this->setpoint.data = setpoint;
            return ErrorCode::Success;
        }
        return ErrorCode::IncompleteData;
    }
    private:
    CartesianPose setpoint;
};
