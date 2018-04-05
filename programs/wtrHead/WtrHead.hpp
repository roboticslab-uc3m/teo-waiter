// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __WAITER_HEAD_HPP__
#define __WAITER_HEAD_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <stdlib.h>

#include "InCvPort.hpp"
#include "InSrPort.hpp"

//instrucciones para el followme
#define VOCAB_FOLLOW_ME VOCAB4('f','o','l','l')
#define VOCAB_STOP_FOLLOWING VOCAB4('s','f','o','l')

//insatrucciones para el waiterbot
#define VOCAB_HELLO_TEO VOCAB4('e','l','o','t')
#define VOCAB_GO_TEO VOCAB4('g','t','e','o')
#define VOCAB_WATER_PLEASE VOCAB4('w','p','l','e')
#define VOCAB_SECOND_MOVEMENT VOCAB4('s','c','m','m')
#define VOCAB_STOP_TEO VOCAB4('s','t','e','o')

using namespace yarp::os;
using namespace yarp::dev;

namespace teo
{

/**
 * @ingroup WtrHead
 *
 * @brief Waiter Head.
 *
 */
class WtrHead : public RFModule {
    public:
        bool configure(ResourceFinder &rf);

    protected:
        /** RFModule interruptModule. */
        virtual bool interruptModule();
        /** RFModule getPeriod. */
        virtual double getPeriod();
        /** RFModule updateModule. */
        virtual bool updateModule();

        InSrPort inDmPort;
        InCvPort inCvPort;

        /** Head Device */
        yarp::dev::PolyDriver headDevice;
        /** Head ControlMode2 Interface */
        yarp::dev::IControlMode2 *headIControlMode2;
        /** Head PositionControl2 Interface */
        yarp::dev::IPositionControl2 *headIPositionControl2;

};

}  // namespace teo

#endif  // __WAITER_HEAD_HPP__
