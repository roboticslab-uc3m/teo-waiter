// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "StateMachine.hpp"

namespace teo
{

/************************************************************************/

bool StateMachine::threadInit() {
    _machineState = 1;
    x = 1;
    z = 1;
    return true;
}

/************************************************************************/

void StateMachine::run() {
    while(!isStopping()) {
            if(_machineState==-1) {
                //ttsSay( notUnderstand );  //-- Sorry, I do not undestand
                _machineState=0;
            } else if(_machineState==0) {
                //ttsSay( repeat );  //-- Please tell me
                _machineState=1;
            } else if(_machineState==1) {
                yarp::os::ConstString inStr = asrListen();
                // Blocking
                _inStrState1 = inStr;
                if( _inStrState1.find( hiTeo ) != yarp::os::ConstString::npos ) _machineState=2;  //-- Hi teo
                else if ( _inStrState1.find( goOnTeo ) != yarp::os::ConstString::npos ) _machineState=3;  //-- Go on teo
                else if ( _inStrState1.find( waterPlease) != yarp::os::ConstString::npos ) _machineState=4;  //-- Water please
                else if ( _inStrState1.find( stopTEO ) != yarp::os::ConstString::npos ) _machineState=5;  //-- Stop now
                else _machineState=-1;

                setSpeakLanguage(_language, _machineState);

            } else if (_machineState==2) {
                ttsSay( hello );  //-- Hi, I am teo, your waiter
                yarp::os::Bottle cmd;
                cmd.addVocab(VOCAB_HELLO_TEO);
                outCmdPortHead->write(cmd);
                outCmdPortManip->write(cmd);
                _machineState=1;
            } else if (_machineState==3) {
                ttsSay( drink );  //-- Are you thirsty
                yarp::os::Bottle cmd;
                cmd.addVocab(VOCAB_GO_TEO);
                outCmdPortHead->write(cmd);
                outCmdPortManip->write(cmd);
                _machineState=1;
            } else if (_machineState==4) {
                ttsSay( take1 );  //-- Okey
                yarp::os::Bottle cmd;
                cmd.addVocab(VOCAB_WATER_PLEASE);
                outCmdPortHead->write(cmd);
                outCmdPortManip->write(cmd);
                _machineState=6;
                yarp::os::Time::delay(15); // ESPERAR PARA SINCRONIZAR //
            } else if (_machineState==6) {
                ttsSay( take2 );  //-- Here you are
                yarp::os::Bottle cmd;
                cmd.addVocab(VOCAB_SECOND_MOVEMENT);
                outCmdPortHead->write(cmd);
                outCmdPortManip->write(cmd);
                _machineState=1;
            } else if (_machineState==5) {
                ttsSay( finish );  //-- Okay, see you later aligator
                yarp::os::Bottle cmd;
                cmd.addVocab(VOCAB_STOP_TEO);
                outCmdPortHead->write(cmd);
                outCmdPortManip->write(cmd);
                _machineState=1;
            } else {
                ttsSay( yarp::os::ConstString("ANOMALY") );
                _machineState=0;
            }
        }
}

/************************************************************************/

void StateMachine::ttsSay(const yarp::os::ConstString& sayConstString) {
    yarp::os::Bottle bOut, bRes;
    bOut.addString("say");
    bOut.addString(sayConstString);
    outTtsPort->write(bOut,bRes);
    printf("[StateMachine] Said: %s [%s]\n", sayConstString.c_str(), bRes.toString().c_str());
    yarp::os::Time::delay(0.5);
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

void StateMachine::setOutTtsPort(yarp::os::RpcClient* outTtsPort) {
    this->outTtsPort = outTtsPort;
}

/************************************************************************/

bool StateMachine::setLanguage(std::string language)
{
    _language = language;
    if("english" == language)    {

        //-- recognition sentences
        hiTeo = std::string ("hi teo");            //state 2
        goOnTeo = std::string ("thank you");      //state 3
        waterPlease = std::string ("give me water");   //state 4
        stopTEO = std::string ("stop teo");        //state 5

        return true;
    }
    else if("spanish" == language)    {

        //-- frases de reconociomiento
        hiTeo = std::string ("hi teo");            //state 2
        goOnTeo = std::string ("thank you");      //state 3
        waterPlease = std::string ("give me water");   //state 4
        stopTEO = std::string ("stop teo");        //state 5

        return true;
    }
    else    {
        printf("error! %s????\n",language.c_str());
        return false;
    }
}

/************************************************************************/

bool StateMachine::setSpeakLanguage(std::string language, int _machineState) {

    if("english" == language)    {

        //-- speak sentences
        notUnderstand = std::string("Sorry, I do not understand."); //state -1
        repeat = std::string("Please tell me."); //state 0
        hello = std::string("Hi, I am teo, your waiter."); //state 2
        drink = std::string("Are you thirsty."); //state 3
        take1 = std::string("Ok."); //state 4
        take2 = std::string("Here you are."); //state 6
        finish = std::string("Okay, see you later aligator."); //state 5

        return true;
    }
    else if("spanish" == language)    {

        //-- frases del habla
        notUnderstand = std::string("Disculpa, no te he entendido."); //state -1
        repeat = std::string("Puedes grepetirlo."); //state 0
        hello = std::string("Hola, me yamo TEO y si quieres, seré tu grobot camarero."); //state 2
        take1 = std::string("Por supuesto."); //state 4
        finish = std::string("Me parece bien. Nos vemos pronto."); //state 5

        if (_machineState == 3)        {
            switch (x) {
            case 1:
                drink = std::string("Que quiere tomar. Mi especialidad es servir cervezas bien fresquitas pero estoy de servicio."); //state 3
                x = 2;
                break;
            case 2:
                drink = std::string("Que quiere tomar. Te puedo ofrecer cualquier bebida que quieras mientras sea, agua."); //state 3
                x = 3;
                break;
            case 3:
                drink = std::string("Que quiere tomar. En nuestro bar, hay de todo y gratis."); //state 3
                x = 1;
                break;
            default:
                break;
            }
        }

        if(_machineState == 4)        {
            switch (z) {
            case 1:
                take2 = std::string("Por favor, sirvase."); //state 6
                z = 2;
                break;
            case 2:
                take2 = std::string("Aqui tiene su boteya."); //state 6
                z = 3;
                break;
            case 3:
                take2 = std::string("Espero que, este a su agrado."); //state 6
                z = 1;
                break;
            default:
                break;
            }
        }

        return true;

    }
    else    {
        printf("error! %s????\n",language.c_str());
        return false;
    }
}

/************************************************************************/

}  // namespace teo
