// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "StateMachine.hpp"

namespace teo
{

/************************************************************************/

bool StateMachine::threadInit() {
    _machineState = 0;
    return true;
}

/************************************************************************/

void StateMachine::run() {
    while(!isStopping()) {
            if(_machineState==-1) {
                ttsSay( yarp::os::ConstString("Sorry, I do not understand.") );
                _machineState=0;
            } else if(_machineState==0) {
                ttsSay( yarp::os::ConstString("Please tell me.") );
                _machineState=1;
            } else if(_machineState==1) {
                yarp::os::ConstString inStr = asrListen();
                // Blocking
                _inStrState1 = inStr;
                if( _inStrState1.find("hello teo") != yarp::os::ConstString::npos ) _machineState=2;
                else if ( _inStrState1.find("go teo") != yarp::os::ConstString::npos ) _machineState=3;
                else if ( _inStrState1.find("water please") != yarp::os::ConstString::npos ) _machineState=4;
                else if ( _inStrState1.find("stop teo") != yarp::os::ConstString::npos ) _machineState=5;
                else _machineState=-1;
            } else if (_machineState==2) {
                ttsSay( yarp::os::ConstString("Hi, I am teo, your waiter.") );
                yarp::os::Bottle cmd;
                cmd.addVocab(VOCAB_HELLO_TEO);
                outCmdPortHead->write(cmd);
                outCmdPortManip->write(cmd);
                _machineState=0;
            } else if (_machineState==3) {
                ttsSay( yarp::os::ConstString("Are you thirsty.") );
                yarp::os::Bottle cmd;
                cmd.addVocab(VOCAB_GO_TEO);
                outCmdPortHead->write(cmd);
                outCmdPortManip->write(cmd);
                _machineState=0;
            } else if (_machineState==4) {
                ttsSay( yarp::os::ConstString("Here you are.") );
                yarp::os::Bottle cmd;
                cmd.addVocab(VOCAB_WATER_PLEASE);
                outCmdPortHead->write(cmd);
                outCmdPortManip->write(cmd);
                _machineState=0;
            } else if (_machineState==5) {
                ttsSay( yarp::os::ConstString("Okay, see you later aligator.") );
                yarp::os::Bottle cmd;
                cmd.addVocab(VOCAB_STOP_TEO);
                outCmdPortHead->write(cmd);
                outCmdPortManip->write(cmd);
                _machineState=0;
            } else {
                ttsSay( yarp::os::ConstString("ANOMALY") );
                _machineState=0;
            }
        }
    /*
    while(!isStopping()) {
        if(_machineState==-1) {
            ttsSay( yarp::os::ConstString("Sorry, I do not know what that is.") );
            _machineState=0;
        } else if(_machineState==0) {
            ttsSay( yarp::os::ConstString("I am ready. Please tell me.") );
            _machineState=1;
        } else if(_machineState==1) {
            yarp::os::ConstString inStr = asrListen();
            // Blocking
            _inStrState1 = inStr;
            if( _inStrState1.find("follow me") != yarp::os::ConstString::npos ) _machineState=2;
            else if ( _inStrState1.find("stop following") != yarp::os::ConstString::npos ) _machineState=3;
            else _machineState=-1;
        } else if (_machineState==2) {
            ttsSay( yarp::os::ConstString("Okay, I will follow you.") );
            yarp::os::Bottle cmd;
            cmd.addVocab(VOCAB_FOLLOW_ME);
            outCmdPortHead->write(cmd);
            outCmdPortManip->write(cmd);
            _machineState=0;
        } else if (_machineState==3) {
            ttsSay( yarp::os::ConstString("Okay, I will stop following you") );
            yarp::os::Bottle cmd;
            cmd.addVocab(VOCAB_STOP_FOLLOWING);
            outCmdPortHead->write(cmd);
            outCmdPortManip->write(cmd);
            _machineState=0;
        } else {
            ttsSay( yarp::os::ConstString("ANOMALY") );
            _machineState=0;
        }
    }*/
}

/************************************************************************/

void StateMachine::ttsSay(const yarp::os::ConstString& sayConstString) {
    yarp::os::Bottle bOut;
    bOut.addString(sayConstString);
    outTtsPort->write(bOut);
    printf("[StateMachine] Said: %s\n", sayConstString.c_str());
    return;
}

/************************************************************************/

yarp::os::ConstString StateMachine::asrListen() {
    yarp::os::Bottle* bIn = inSrPort->read(true);  // shouldWait
    printf("[StateMachine] Listened: %s\n", bIn->toString().c_str());
    return bIn->get(0).asString();
}

/************************************************************************/

int StateMachine::getMachineState() {
    return _machineState;
}

/************************************************************************/

void StateMachine::setInSrPort(yarp::os::BufferedPort<yarp::os::Bottle>* inSrPort) {
    this->inSrPort = inSrPort;
}

/************************************************************************/

void StateMachine::setOutCmdPortHead(yarp::os::Port* outCmdPort) {
    this->outCmdPortHead = outCmdPort;
}

/************************************************************************/

void StateMachine::setOutCmdPortManip(yarp::os::Port* outCmdPort) {
    this->outCmdPortManip = outCmdPort;
}

/************************************************************************/

void StateMachine::setOutTtsPort(yarp::os::Port* outTtsPort) {
    this->outTtsPort = outTtsPort;
}

/************************************************************************/

}  // namespace teo
