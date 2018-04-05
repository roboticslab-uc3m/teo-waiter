// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "InDiaPort.hpp"

namespace teo
{

/************************************************************************/

bool InDiaPort::read(ConnectionReader& connection) {
    Bottle in;
    bool ok = in.read(connection);
    if (!ok) return false;
    switch ( in.get(0).asVocab() ) {
        case VOCAB_HELLO_TEO:
            printf("follow\n");
            inCvPortPtr->setFollow(true);
            break;
        case VOCAB_STOP_TEO:
            printf("stopFollowing\n");
            inCvPortPtr->setFollow(false);
            break;
        default:
            break;

        return true;
    }
}

/************************************************************************/

}  // namespace teo
