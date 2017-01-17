// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "WtrHead.hpp"
#include "InCvPort.hpp"

namespace teo
{

/************************************************************************/

bool WtrHead::configure(ResourceFinder &rf) {

    //ConstString fileName(DEFAULT_FILE_NAME);
    
    printf("--------------------------------------------------------------\n");
    if (rf.check("help")) {
        printf("WtrHead options:\n");
        printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        //printf("\t--file (default: \"%s\")\n",fileName.c_str());
    }
    //if (rf.check("file")) fileName = rf.find("file").asString();
    //printf("WaiterExecHead using file: %s\n",fileName.c_str());

    printf("--------------------------------------------------------------\n");
    if(rf.check("help")) {
        ::exit(1);
    }

    //
    Property headOptions;
    headOptions.put("device","remote_controlboard");
    headOptions.put("local","/waiterHead/head");
    headOptions.put("remote","/teo/head");
    headDevice.open(headOptions);
    if( ! headDevice.isValid() ) {
        printf("head remote_controlboard instantiation not worked.\n");
        return false;
    }

    if( ! headDevice.view(iPositionControl) ) {
        printf("view(iVelocityControl) not worked.\n");
        return false;
    }
    inCvPort.setIPositionControl(iPositionControl);
    iPositionControl->setPositionMode();

    //-----------------OPEN LOCAL PORTS------------//
    inSrPort.setInCvPortPtr(&inCvPort);
    inCvPort.useCallback();
    inSrPort.useCallback();
    inSrPort.open("/wtrHead/dialogue/command:i");
    inCvPort.open("/wtrHead/cvBottle/state:i");

    return true;
}

/************************************************************************/
double WtrHead::getPeriod() {
    return 2.0;  // Fixed, in seconds, the slow thread that calls updateModule below
}

/************************************************************************/
bool WtrHead::updateModule() {
    //printf("StateMachine in state [%d]. WaiterExecHead alive...\n", stateMachine.getMachineState());
    return true;
}

/************************************************************************/

bool WtrHead::interruptModule() {
    printf("WtrHead closing...\n");
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
