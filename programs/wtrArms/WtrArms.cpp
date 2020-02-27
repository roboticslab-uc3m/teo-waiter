// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "WtrArms.hpp"

namespace teo
{

/************************************************************************/

bool WtrArms::configure(yarp::os::ResourceFinder &rf)
{
    std::string robot = rf.check("robot",yarp::os::Value(DEFAULT_ROBOT),"name of /robot to be used").asString();

    printf("--------------------------------------------------------------\n");
    if (rf.check("help"))
    {
        printf("WtrArms options:\n");
        printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        printf("\t--robot: %s [%sSim]\n",robot.c_str(),DEFAULT_ROBOT);
        ::exit(0);
    }

    std::string WtrArmsStr("/wtrArms");

    // ------ LEFT ARM -------
    yarp::os::Property leftArmOptions;
    leftArmOptions.put("device","remote_controlboard");
    leftArmOptions.put("remote",robot+"/leftArm");
    leftArmOptions.put("local",WtrArmsStr+robot+"/leftArm");
    leftArmDevice.open(leftArmOptions);
    if(!leftArmDevice.isValid()) {
      printf("robot leftArm device not available.\n");
      leftArmDevice.close();
      yarp::os::Network::fini();
      return false;
    }

    if (!leftArmDevice.view(leftArmIControlMode2) ) { // connecting our device with "control mode 2" interface, initializing which control mode we want (position)
        printf("[warning] Problems acquiring leftArmPos interface\n");
        return false;
    } else printf("[success] Acquired leftArmPos interface\n");
    if (!leftArmDevice.view(leftArmIPositionControl2) ) { // connecting our device with "position control 2" interface (configuring our device: speed, acceleration... and sending joint positions)
        printf("[warning] Problems acquiring leftArmIControlMode2 interface\n");
        return false;
    } else printf("[success] Acquired leftArmIControlMode2 interface\n");    

    // ------ RIGHT ARM -------
    yarp::os::Property rightArmOptions;
    rightArmOptions.put("device","remote_controlboard");
    rightArmOptions.put("remote",robot+"/rightArm");
    rightArmOptions.put("local",WtrArmsStr+robot+"/rightArm");
    rightArmDevice.open(rightArmOptions);
    if(!rightArmDevice.isValid()) {
      printf("robot rightArm device not available.\n");
      rightArmDevice.close();
      yarp::os::Network::fini();
      return false;
    }

    if (!rightArmDevice.view(rightArmIControlMode2) ) { // connecting our device with "control mode 2" interface, initializing which control mode we want (position)
        printf("[warning] Problems acquiring rightArmPos interface\n");
        return false;
    } else printf("[success] Acquired rightArmPos interface\n");
    if (!rightArmDevice.view(rightArmIPositionControl2) ) { // connecting our device with "position control 2" interface (configuring our device: speed, acceleration... and sending joint positions)
        printf("[warning] Problems acquiring rightArmIControlMode2 interface\n");
        return false;
    } else printf("[success] Acquired rightArmIControlMode2 interface\n");

    // ------ RIGHT HAND -------
    yarp::os::Property rightHandOptions;
    rightHandOptions.put("device","remote_controlboard");
    rightHandOptions.put("remote",robot+"/rightHand");
    rightHandOptions.put("local",WtrArmsStr+robot+"/rightHand");
    rightHandDevice.open(rightHandOptions);
    if(!rightHandDevice.isValid()) {
      printf("robot rightHand device not available.\n");
      rightHandDevice.close();
      yarp::os::Network::fini();
      return false;
    }

    if (!rightHandDevice.view(rightHandIPWMControl) ) {
        printf("[warning] Problems acquiring rightHandIPWMControl interface\n");
        return false;
    } else printf("[success] Acquired rightHandIPWMControl interface\n");

    //------ Set control modes -------- //
    int leftArmAxes;
    leftArmIPositionControl2->getAxes(&leftArmAxes);
    std::vector<int> leftArmControlModes(leftArmAxes,VOCAB_CM_POSITION);
    if(! leftArmIControlMode2->setControlModes( leftArmControlModes.data() )){
        printf("[warning] Problems setting position control mode of: left-arm\n");
        return false;
    }

    int rightArmAxes;
    rightArmIPositionControl2->getAxes(&rightArmAxes);
    std::vector<int> rightArmControlModes(rightArmAxes,VOCAB_CM_POSITION);
    if(! rightArmIControlMode2->setControlModes(rightArmControlModes.data())){
        printf("[warning] Problems setting position control mode of: right-arm\n");
        return false;
    }

    // -- Configuring Speeds and Accelerations

    // -- Arms
    std::vector<double> armSpeeds(6,25.0); // 7,30.0
    std::vector<double> armAccelerations(6,25.0); // 7,30.0

    // -- configuring..

    if(!rightArmIPositionControl2->setRefSpeeds(armSpeeds.data())){
        printf("[Error] Problems setting reference speed on right-arm joints.\n");
        return false;
    }
    if(!leftArmIPositionControl2->setRefSpeeds(armSpeeds.data())){
        printf("[Error] Problems setting reference speed on left-arm joints.\n");
        return false;
    }

    if(!rightArmIPositionControl2->setRefAccelerations(armAccelerations.data())){
        printf("[Error] Problems setting reference acceleration on right-arm joints.\n");
        return false;
    }
    if(!leftArmIPositionControl2->setRefAccelerations(armAccelerations.data())){
        printf("[Error] Problems setting reference acceleration on left-arm joints.\n");
        return false;
    }

    phase = 0;

    inDialogPort.open("/wtrArms/dialogue/rpc:s");
    inDialogPort.setReader(*this);  //-- Callback reader: avoid need to call inSrPort.read().

    return this->start();  //-- Start the thread (calls run).
}

/************************************************************************/

bool WtrArms::interruptModule()
{
    this->stop();
    inDialogPort.interrupt();
    leftArmDevice.close();
    return true;
}

/************************************************************************/

double WtrArms::getPeriod()
{
    return 4.0; // Fixed, in seconds, the slow thread that calls updateModule below
}

/************************************************************************/

bool WtrArms::updateModule()
{
    printf("Entered updateModule...\n");

    return true;
}

/************************************************************************/

bool WtrArms::movingArmJoints(std::vector<double>& leftArmQ, std::vector<double> &rightArmQ, double rightHandPwm)
{
    rightArmIPositionControl2->positionMove( rightArmQ.data() );
    leftArmIPositionControl2->positionMove( leftArmQ.data() );
    rightHandIPWMControl->setRefDutyCycle(0, rightHandPwm);

    //printf("Waiting for right arm.");
    bool doneR = false; // checking the position move
    while((!doneR)&&(!Thread::isStopping()))
    {
        rightArmIPositionControl2->checkMotionDone(&doneR);
        yarp::os::Time::delay(0.1);
    }

    //printf("Waiting for left arm.");
    bool doneL = false;
    while((!doneL)&&(!Thread::isStopping()))
    {
        leftArmIPositionControl2->checkMotionDone(&doneL);
        yarp::os::Time::delay(0.1);
    }

    return true;
}

/************************************************************************/

bool WtrArms::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    bool ok = in.read(connection);    
    if (!ok) return false;
    printf("[WtrArms] Got %s\n", in.toString().c_str());

    switch ( in.get(0).asVocab() ) {
        case VOCAB_HELLO_TEO:
            printf("HELLO_TEO\n");
            state = VOCAB_HELLO_TEO;
            break;
        case VOCAB_GO_TEO:
            printf("GO_TEO\n");
            state = VOCAB_GO_TEO;
            break;
        case VOCAB_WATER_PLEASE:
            printf("WATER_PLEASE\n");
            state = VOCAB_WATER_PLEASE;
            break;
        case VOCAB_SECOND_MOVEMENT:
            printf("WATER_SECOND_MOVEMENT\n");
            state = VOCAB_SECOND_MOVEMENT;
            break;
        case VOCAB_STOP_TEO:
            printf("STOP_TEO\n");
            state = VOCAB_STOP_TEO;
            break;
        default:
            break;
        return true;
    }

}

/************************************************************************/

void WtrArms::run()
{
    while( !Thread::isStopping() )
    {
        switch ( state )    {
            case VOCAB_HELLO_TEO: {//VOCAB_HELLO_TEO

                printf("Salute\n");
                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -20;
                leftArmQ[1] = 0;
                leftArmQ[2] = 0;
                leftArmQ[3] = -60;
                leftArmQ[4] = 0;
                leftArmQ[5] = -10;
                movingArmJoints(leftArmQ,rightArmQ);
                }

                break;
            }

            case VOCAB_GO_TEO: {//VOCAB_GO_TEO

                printf("stability\n");
                if (phase==0 && state) {
                    printf("begin MOVE TO Pa POSITION\n");
                    double Pa[7] = {-30, 40, 0, -70, -34, 10};
                    leftArmIPositionControl2->positionMove(Pa);
                    yarp::os::Time::delay(3); // 4
                    phase=1;
                } // MOVIMIENTO NUMERO 1

                if (phase==1 && state) {
                    printf("begin MOVE TO Pb POSITION\n");
                    double Pb[7] = {-20, 30, 0, -80, -28, 10};
                    leftArmIPositionControl2->positionMove(Pb);
                    yarp::os::Time::delay(2); // 3
                    phase=2;
                } // MOVIMIENTO NUMERO 2

                if (phase==2 && state) {
                    printf("begin MOVE TO Pc POSITION\n");
                    double Pc[7] = {-30, -10, 0, -70, 9, 10};
                    leftArmIPositionControl2->positionMove(Pc);
                    yarp::os::Time::delay(3); //4
                    phase=0;
                } // MOVIMIENTO NUMERO 3

                break;
            }

            case VOCAB_WATER_PLEASE: {//VOCAB_WATER_PLEASE

                printf("giving water - 1st part\n");

    //Preparing position:

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -30;
                leftArmQ[1] = -10;
                leftArmQ[2] = 0.0;
                leftArmQ[3] = -70;
                leftArmQ[4] = 10;
                leftArmQ[5] = 9;

                rightArmQ[0] = 63.0931434631348;
                rightArmQ[1] = -29.8594055175781;
                rightArmQ[2] = -55.1669616699219;
                rightArmQ[3] = 83.3040390014648;
                rightArmQ[4] = 54.2179260253906;
                rightArmQ[5] = 0.0;

                movingArmJoints(leftArmQ,rightArmQ, 100.0);
                }


    // PUNTO P1  (63.0931434631348 -29.8594055175781 -55.1669616699219 83.3040390014648 54.2179260253906 0.0 1023.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -42.0738220214844;
                leftArmQ[1] = -15.5360412597656;
                leftArmQ[2] = 39.0158157348633;
                leftArmQ[3] = -55.8699645996094;
                leftArmQ[4] = -17.9085998535156;
                leftArmQ[5] = 6.85412979125977;

                rightArmQ[0] = 63.0931434631348;
                rightArmQ[1] = -29.8594055175781;
                rightArmQ[2] = -55.1669616699219;
                rightArmQ[3] = 83.3040390014648;
                rightArmQ[4] = 54.2179260253906;
                rightArmQ[5] = 0.0;

                movingArmJoints(leftArmQ,rightArmQ, 100.0);
                }

    // PUNTO P2  (63.1810188293457 -1.03689575195312 -55.0791015625 83.3040390014648 54.2179260253906 0.0 1023.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -42.0738220214844;
                leftArmQ[1] = -15.5360412597656;
                leftArmQ[2] = 39.0158157348633;
                leftArmQ[3] = -55.8699645996094;
                leftArmQ[4] = -17.9085998535156;
                leftArmQ[5] = 6.85412979125977;

                rightArmQ[0] = 63.1810188293457;
                rightArmQ[1] = -1.03689575195312;
                rightArmQ[2] = -55.0791015625;
                rightArmQ[3] = 83.3040390014648 ;
                rightArmQ[4] = 54.2179260253906;
                rightArmQ[5] = 0.0;

                movingArmJoints(leftArmQ,rightArmQ, 100.0);
                }

    // PUNTO P3  (63.0931434631348 -1.03689575195312 -55.0791015625 83.3040390014648 54.2179260253906 -24.8506164550781 1023.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -42.0738220214844;
                leftArmQ[1] = -15.5360412597656;
                leftArmQ[2] = 39.0158157348633;
                leftArmQ[3] = -55.8699645996094;
                leftArmQ[4] = -17.9085998535156;
                leftArmQ[5] = 6.85412979125977;

                rightArmQ[0] = 63.0931434631348;
                rightArmQ[1] = -1.03689575195312;
                rightArmQ[2] = -55.0791015625 ;
                rightArmQ[3] = 83.3040390014648 ;
                rightArmQ[4] = 54.2179260253906 ;
                rightArmQ[5] = -24.00 ;

                movingArmJoints(leftArmQ,rightArmQ,100.0);
                }

                 yarp::os::Time::delay(1);

    // Close right hand


                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -42.0738220214844;
                leftArmQ[1] = -15.5360412597656;
                leftArmQ[2] = 39.0158157348633;
                leftArmQ[3] = -55.8699645996094;
                leftArmQ[4] = -17.9085998535156;
                leftArmQ[5] = 6.85412979125977;

                rightArmQ[0] = 63.0931434631348;
                rightArmQ[1] = -1.03689575195312;
                rightArmQ[2] = -55.0791015625 ;
                rightArmQ[3] = 83.3040390014648 ;
                rightArmQ[4] = 54.2179260253906 ;
                rightArmQ[5] = -24.00 ;

                movingArmJoints(leftArmQ,rightArmQ,-100.0);
                }

                yarp::os::Time::delay(1);
                break;
            }

            case VOCAB_SECOND_MOVEMENT: {//VOCAB_SECOND_MOVEMENT

                printf("giving water - 2nd part\n");

    // Punto P5:  (59.1388397216797 -17.3813781738281 -21.6871643066406 63.444637298584 24.8681888580322 -20.9841918945312 -1023.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -20;
                leftArmQ[1] = -10;
                leftArmQ[2] = 0;
                leftArmQ[3] = -50; // -60
                leftArmQ[4] = 10;
                leftArmQ[5] = -10;

                rightArmQ[0] = 59.1388397216797;
                rightArmQ[1] = -17.3813781738281;
                rightArmQ[2] = -21.6871643066406;
                rightArmQ[3] = 63.444637298584;
                rightArmQ[4] = 24.8681888580322;
                rightArmQ[5] = -20.9841918945312;

                movingArmJoints(leftArmQ,rightArmQ,-100.0);
                }

                yarp::os::Time::delay(1);

    // Open right hand


                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -20;
                leftArmQ[1] = -10;
                leftArmQ[2] = 0;
                leftArmQ[3] = -60;
                leftArmQ[4] = 10;
                leftArmQ[5] = -10;

                rightArmQ[0] = 59.1388397216797;
                rightArmQ[1] = -17.3813781738281;
                rightArmQ[2] = -21.6871643066406;
                rightArmQ[3] = 63.444637298584;
                rightArmQ[4] = 24.8681888580322;
                rightArmQ[5] = -20.9841918945312;

                movingArmJoints(leftArmQ,rightArmQ,100.0);
                }

                yarp::os::Time::delay(1);

    // Relax right hand

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -20;
                leftArmQ[1] = -10;
                leftArmQ[2] = 0;
                leftArmQ[3] = -60;
                leftArmQ[4] = 10;
                leftArmQ[5] = -10;

                rightArmQ[0] = 0.0;
                rightArmQ[1] = 0.0;
                rightArmQ[2] = 0.0;
                rightArmQ[3] = 0.0;
                rightArmQ[4] = 0.0;
                rightArmQ[5] = 0.0;

                movingArmJoints(leftArmQ,rightArmQ,-100.0);
                }

                yarp::os::Time::delay(1);

                state = VOCAB_HELLO_TEO;
                break;
            }

            case VOCAB_STOP_TEO: {//VOCAB_STOP_TEO

                printf("homing\n");

                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                movingArmJoints(leftArmQ,rightArmQ);

                break;
            }
            default:
                break;
        }

    }

    return;
}

/************************************************************************/

}  // namespace teo

