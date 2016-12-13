// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __WAITER_ARMS_HPP__
#define __WAITER_ARMS_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#define DEFAULT_ROBOT "/teo"

#define VOCAB_FOLLOW_ME VOCAB4('f','o','l','l')

#define VOCAB_STATE_SALUTE VOCAB4('s','a','l','u')
#define VOCAB_STATE_ARM_SWINGING VOCAB4('s','w','i','n')
#define VOCAB_STOP_FOLLOWING VOCAB4('s','f','o','l')

namespace teo
{

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
     /** Left Arm Position Interface */
     yarp::dev::IPositionControl *leftArmIPositionControl;

     /** Right Arm Device */
     yarp::dev::PolyDriver rightArmDevice;
     /** Right Arm Position Interface */
     yarp::dev::IPositionControl *rightArmIPositionControl;

     /** Phase of arm swing movement */
     bool phase;

     /** Arm Joints Move And Wait */
     bool movingArmJoints (std::vector<double>& leftArmQ, std::vector<double>& rightArmQ);

     /** State */
     int state;

     /** Input port from speech recognition */
     yarp::os::Port inSrPort;
     /** Treats data received from input port from speech recognition */
     virtual bool read(yarp::os::ConnectionReader& connection);

     /** Thread run */
     virtual void run();

};

}  // namespace teo

#endif  // __WAITER_ARMS_HPP__
