// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __WAITER_EXECUTION_CORE_HPP__
#define __WAITER_EXECUTION_CORE_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <stdlib.h>

#include "InCvPort.hpp"
//#include "InSrPort.hpp"

#define VOCAB_FOLLOW_ME VOCAB4('f','o','l','l')
#define VOCAB_STOP_FOLLOWING VOCAB4('s','f','o','l')

using namespace yarp::os;
using namespace yarp::dev;

namespace teo
{

/**
 * @ingroup WaiterExecutionCore
 *
 * @brief Waiter Execution Core.
 *
 */
class WaiterExecutionCore : public RFModule {
    public:
        bool configure(ResourceFinder &rf);

    protected:
     //   InSrPort inSrPort;
        InCvPort inCvPort;
        yarp::dev::PolyDriver headDevice;
        yarp::dev::IPositionControl *iPositionControl;
        yarp::dev::IVelocityControl *iVelocityControl;

        bool interruptModule();
        double getPeriod();
        bool updateModule();

};

}  // namespace teo

#endif  // __WAITER_EXECUTION_CORE_HPP__
