// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __IN_DIA_PORT_HPP__
#define __IN_DIA_PORT_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <stdlib.h>

#include "InCvPort.hpp"

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

namespace teo
{

/**
 * @ingroup WaiterExecutionCore
 *
 * @brief Input port of speech recognition data.
 *
 */
class InDiaPort : public yarp::os::PortReader {
    public:
        void setInCvPortPtr(InCvPort *inCvPortPtr) {
            this->inCvPortPtr = inCvPortPtr;
        }

    protected:
        /** Treats data received from input port from speech recognition */
        virtual bool read(ConnectionReader& connection);

        InCvPort* inCvPortPtr;
};

}  // namespace teo

#endif  // __IN_DIA_PORT_HPP__
