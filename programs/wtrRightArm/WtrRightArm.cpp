// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "WtrRightArm.hpp"

namespace teo
{

/************************************************************************/

bool WtrRightArm::configure(ResourceFinder &rf) {

    printf("--------------------------------------------------------------\n");
    if (rf.check("help")) {
        printf("WtrRightArm options:\n");
        printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        //printf("\t--file (default: \"%s\")\n",fileName.c_str());
    }

    printf("--------------------------------------------------------------\n");
    if(rf.check("help")) {
        ::exit(1);
    }

    //
    Property rightArmOptions;
    rightArmOptions.put("device","remote_controlboard");
    rightArmOptions.put("local","/waiterRightArm/teo/leftArm");
    rightArmOptions.put("remote","/teo/RightArm");
    rightArmDevice.open(rightArmOptions);
    if( ! leftArmDevice.isValid() ) {
        printf("rightArm remote_controlboard instantiation not worked.\n");
        return false;
    }
    if( ! rightArmDevice.view(iPositionControl) ) {
        printf("view(iPositionControl) not worked.\n");
        return false;
    }
    inCvPort.setIPositionControl(iPositionControl);

    //-----------------OPEN LOCAL PORTS------------//
    inSrPort.setInCvPortPtr(&inCvPort);
    inCvPort.useCallback();
    inSrPort.useCallback();
    inSrPort.open("/waiterRightArm/DialogueManager/command:i");
    inCvPort.open("/waiterRightArm/cvBottle/state:i");

    return true;
}

/************************************************************************/
double WtrRightArm::getPeriod() {
    return 2.0;  // Fixed, in seconds, the slow thread that calls updateModule below
}

/************************************************************************/
bool WtrRightArm::updateModule() {
    //printf("StateMachine in state [%d]. WtrRightArm alive...\n", stateMachine.getMachineState());
    return true;
}

/************************************************************************/

bool WtrRightArm::interruptModule() {
    printf("wtrRightArm closing...\n");
    inCvPort.disableCallback();
    inSrPort.disableCallback();
    inCvPort.interrupt();
    inSrPort.interrupt();
    inCvPort.close();
    inSrPort.close();
    return true;
}

/************************************************************************/

}  // namespace teo
