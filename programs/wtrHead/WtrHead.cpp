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

    // ------ HEAD -------
    Property headOptions;
    headOptions.put("device","remote_controlboard");
    headOptions.put("local","/waiterHead/head");
    headOptions.put("remote","/teo/head");
    headDevice.open(headOptions);

    if (!headDevice.view(headIControlMode2) ) { // connecting our device with "control mode 2" interface, initializing which control mode we want (position)
        printf("[warning] Problems acquiring headIControlMode2 interface\n");
        return false;
    } else printf("[success] Acquired headIControlMode2 interface\n");
    if (!headDevice.view(headIPositionControl2) ) { // connecting our device with "position control 2" interface (configuring our device: speed, acceleration... and sending joint positions)
        printf("[warning] Problems acquiring headIPositionControl2 interface\n");
        return false;
    } else printf("[success] Acquired headIPositionControl2 interface\n");

    //-- Set control modes
    int headAxes;
    headIPositionControl2->getAxes(&headAxes);
    std::vector<int> headControlModes(headAxes,VOCAB_CM_POSITION);
    if(! headIControlMode2->setControlModes( headControlModes.data() )){
        printf("[warning] Problems setting position control mode of: head\n");
        return false;
    }

    inCvPort.setIPositionControl(headIPositionControl2);
    //iPositionControl->setPositionMode();

    //-----------------OPEN LOCAL PORTS------------//
    inDiaPortProcessor.setInCvPortPtr(&inCvPort);
    inDiaPort.setReader(inDiaPortProcessor);
    inDiaPort.open("/wtrHead/dialogue/rpc:s");

    inCvPort.useCallback();
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
    inCvPort.interrupt();
    inDiaPort.interrupt();
    inCvPort.close();
    inDiaPort.close();
    return true;
}

/************************************************************************/

}  // namespace teo
