// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "WtrDialogue.hpp"

namespace teo
{

/************************************************************************/
bool WtrDialogue::configure(yarp::os::ResourceFinder &rf) {

    std::string language = rf.check("language",yarp::os::Value(DEFAULT_LANGUAGE),"language to be used").asString();

    printf("--------------------------------------------------------------\n");
    if (rf.check("help")) {
        printf("WtrDialogue options:\n");
        printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        printf("\t--language (default: \"%s\")\n",language.c_str());
    }
    printf("WtrDialogue using language: %s\n",language.c_str());

    printf("--------------------------------------------------------------\n");
    if(rf.check("help")) {
        ::exit(1);
    }

    //-----------------OPEN LOCAL PORTS------------//
    outCmdPortHead.open("/wtrDialogue/moveHead/rpc:c"); // -- head movements (output)
    outCmdPortManip.open("/wtrDialogue/moveArms/rpc:c"); // -- arms movements (output)
    outTtsPort.open("/wtrDialogue/tts/rpc:c");      // -- talking (output)
    outRecognitionPort.open("/wtrDialogue/setDictionary/rpc:c");    // -- setDictionary (client)
    inSrPort.open("/wtrDialogue/speRecog/speech:i");                // -- words (input)

    stateMachine.setOutCmdPortHead(&outCmdPortHead);
    stateMachine.setOutCmdPortManip(&outCmdPortManip);
    stateMachine.setOutTtsPort(&outTtsPort);
    stateMachine.setInSrPort(&inSrPort);

    yarp::os::Network::connect("/wtrDialogue/setDictionary/rpc:c", "/speechRecognition/rpc:s");
    while(1){
        if(outTtsPort.getOutputCount() > 0) break;
        printf("Waiting for '/wtrDialogue/tts/rpc:c' to be connected to something...\n");
        yarp::os::Time::delay(0.5);
    }

    //-----------------ACTIVE IDIOM FOR DICTIONARY------------//
    yarp::os::Bottle bOut,  bRec;
    bOut.addString("setLanguage");
    bRec.addString("setDictionary");
    bRec.addString("waiter");

    if( language == "english" )    {
        bOut.addString("mb-en1");
        bRec.addString(language );
    }
    else if ( language == "spanish" )    {
        bOut.addString("mb-es1");
        bRec.addString(language);
    }
    else    {
        printf("Language not found. Please use '--language english' or '--language spanish'");
        return false;
    }

    outTtsPort.write(bOut);
    outRecognitionPort.write(bRec);

    stateMachine.setLanguage(language);

    stateMachine.start();

    return true;
}

/************************************************************************/
double WtrDialogue::getPeriod() {
    return 2.0;  // Fixed, in seconds, the slow thread that calls updateModule below
}

/************************************************************************/
bool WtrDialogue::updateModule() {
    printf("StateMachine in state [%d]. WtrDialogue alive...\n", stateMachine.getMachineState());
    return true;
}

/************************************************************************/
bool WtrDialogue::interruptModule() {
    printf("WtrDialogue closing...\n");
    inSrPort.interrupt();
    outTtsPort.interrupt();
    outCmdPortHead.interrupt();
    outRecognitionPort.interrupt();
    stateMachine.stop();
    inSrPort.close();
    outTtsPort.close();
    outCmdPortHead.close();
    outRecognitionPort.close();

    return true;
}

/************************************************************************/

}  // namespace teo
