// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __WAITER_ARMS_HPP__
#define __WAITER_ARMS_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#define DEFAULT_ROBOT "/teo"

#define VOCAB_FOLLOW_ME VOCAB4('f','o','l','l')
#define VOCAB_STOP_FOLLOWING VOCAB4('s','f','o','l')

//instrucciones para el waiterbot
#define VOCAB_HELLO_TEO VOCAB4('e','l','o','t')
#define VOCAB_GO_TEO VOCAB4('g','t','e','o')
#define VOCAB_WATER_PLEASE VOCAB4('w','p','l','e')
#define VOCAB_SECOND_MOVEMENT VOCAB4('s','c','m','m')
#define VOCAB_STOP_TEO VOCAB4('s','t','e','o')

namespace teo
{

/**
 * @ingroup waiter_programs
 *
 * @brief waiter Arms.
 *
 */
class WtrArms : public yarp::os::RFModule, public yarp::os::PortReader, public yarp::os::Thread
{
public:
     virtual bool configure(yarp::os::ResourceFinder &rf);
private:
     /** RFModule interruptModule. */
     virtual bool interruptModule();
     /** RFModule getPeriod. */
     virtual double getPeriod();
     /** RFModule updateModule. */
     virtual bool updateModule();

    /** Left Arm Device */
    yarp::dev::PolyDriver leftArmDevice;
    /** Left Arm ControlMode2 Interface */
    yarp::dev::IControlMode2 *leftArmIControlMode2;
    /** Left Arm PositionControl2 Interface */
    yarp::dev::IPositionControl2 *leftArmIPositionControl2;

    /** Right Arm Device */
    yarp::dev::PolyDriver rightArmDevice;
    /** Right Arm ControlMode2 Interface */
    yarp::dev::IControlMode2 *rightArmIControlMode2;
    /** Right Arm PositionControl2 Interface */
     yarp::dev::IPositionControl2 *rightArmIPositionControl2;

     /** Right Hand */
     yarp::dev::PolyDriver rightHandDevice;
     /** Right Arm PWMControl Interface */
     yarp::dev::IPWMControl *rightHandIPWMControl;

     /** Phase of arm stability movement */
     int phase;

     /** Arm Joints Move And Wait */
     bool movingArmJoints (std::vector<double>& leftArmQ, std::vector<double>& rightArmQ, double rightHandPwm = 0.0);

     /** State */
     int state;

     /** Input port from speech recognition */
     yarp::os::RpcServer inDialogPort;

     /** Treats data received from input port from speech recognition */
     virtual bool read(yarp::os::ConnectionReader& connection);

     /** Thread run */
     virtual void run();

};

}  // namespace teo

#endif  // __WAITER_ARMS_HPP__
