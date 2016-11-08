// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "WaiterDialogueManager.hpp"

namespace teo
{

/************************************************************************/

bool WaiterDialogueManager::configure(yarp::os::ResourceFinder &rf) {

    std::string language = rf.check("language",yarp::os::Value(DEFAULT_LANGUAGE),"language to be used").asString();

    printf("--------------------------------------------------------------\n");
    if (rf.check("help")) {
        printf("WaiterDialogueManager options:\n");
        printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        printf("\t--language (default: \"%s\")\n",language.c_str());
    }
    printf("WaiterDialogueManager using language: %s\n",language.c_str());

    printf("--------------------------------------------------------------\n");
    if(rf.check("help")) {
        ::exit(1);
    }

    //-----------------OPEN LOCAL PORTS------------//
    outCmdPortHead.open("/waiterDialMan/Manip/command:o");
    outCmdPortManip.open("/waiterDialMan/Head/command:o");
    outTtsPort.open("/waiterDialMan/tts/rpc:c");
    outRecognitionPort.open("/waiterDialMan/speechRecognition/setDictionary/rpc:c");
    inSrPort.open("/waiterDialMan/speechRecognition:i");
    stateMachine.setOutCmdPortHead(&outCmdPortHead);
    stateMachine.setOutCmdPortManip(&outCmdPortManip);
    stateMachine.setOutTtsPort(&outTtsPort);
    stateMachine.setInSrPort(&inSrPort);
    yarp::os::Network::connect("/waiterDialMan/speechRecognition/setDictionary/rpc:c", "/speechRecognition/rpc:s");

    while(1){
        if(outTtsPort.getOutputCount() > 0) break;
        printf("Waiting for '/waiterDialMan/tts/rpc:c' to be connected to something...\n");
        yarp::os::Time::delay(0.5);
    }
    yarp::os::Bottle bOut,  bRec;

    bOut.addString("setLanguage");
    bRec.addString("setDictionary");
    bRec.addString("waiter");

    if( language == "english" )
    {
        bOut.addString("mb-en1");
        bRec.addString(language );
    }
    else if ( language == "spanish" )
    {
        bOut.addString("mb-es1");
        bRec.addString(language);
    }
    else
    {
        printf("Language not found. Please use '--language english' or '--language spanish'");
        return false;
    }
    printf("%s\n", bOut.toString().c_str());
    outTtsPort.write(bOut);
    printf("%s\n", bRec.toString().c_str());
    outRecognitionPort.write(bRec);

    stateMachine.setLanguage(language);
    stateMachine.start();

    return true;
}

/************************************************************************/
double WaiterDialogueManager::getPeriod() {
    return 2.0;  // Fixed, in seconds, the slow thread that calls updateModule below
}

/************************************************************************/
bool WaiterDialogueManager::updateModule() {
    printf("StateMachine in state [%d]. WaiterDialogueManager alive...\n", stateMachine.getMachineState());
    return true;
}

/************************************************************************/

bool WaiterDialogueManager::interruptModule() {
    printf("WaiterDialogueManager closing...\n");
    inSrPort.interrupt();
    outTtsPort.interrupt();
    stateMachine.stop();
    inSrPort.close();
    outTtsPort.close();
    return true;
}

/************************************************************************/

}  // namespace teo
