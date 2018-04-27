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

bool WtrArms::movingArmJoints(std::vector<double>& leftArmQ, std::vector<double> &rightArmQ)
{
    rightArmIPositionControl2->positionMove( rightArmQ.data() );
    leftArmIPositionControl2->positionMove( leftArmQ.data() );

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
                    double Pa[7] = {-30, 40, 0, -70, -40, 10, 0};
                    leftArmIPositionControl2->positionMove(Pa);
                    yarp::os::Time::delay(4);
                    phase=1;
                } // MOVIMIENTO NUMERO 1

                if (phase==1 && state) {
                    printf("begin MOVE TO Pb POSITION\n");
                    double Pb[7] = {-20, 30, 0, -80, -30, 10, 0};
                    leftArmIPositionControl2->positionMove(Pb);
                    yarp::os::Time::delay(3);
                    phase=2;
                } // MOVIMIENTO NUMERO 2

                if (phase==2 && state) {
                    printf("begin MOVE TO Pc POSITION\n");
                    double Pc[7] = {-30, -10, 0, -70, 10, 10, 0};
                    leftArmIPositionControl2->positionMove(Pc);
                    yarp::os::Time::delay(4);
                    phase=0;
                } // MOVIMIENTO NUMERO 3

                break;
            }

            case VOCAB_WATER_PLEASE: {//VOCAB_WATER_PLEASE

                printf("giving water - 1st part\n");

          /*// puntos de aproximacion
            // Punto P5:  (66.080841 5.975395 -26.871704 35.413006 -70.808441 3.163445 0.0)
            // Punto PF:  (58.17223 0.175747 -55.782074 71.616875 -38.82251 28.20738 0.0)
            // Punto P4:  (57.732864 0.790861 -44.797882 71.968369 -38.91037 38.927944 0.0)
            // PUNTO P3:  (57.820736 -5.606323 -44.797882 71.968369 -38.91037 45.782074 0.0)
            // PUNTO P2:  (57.469242 -12.108978 -41.54657 71.968369 -38.99826 53.427063 0.0)
            // PUNTO P1:  (53.60281 -11.933228 -16.063263 49.03339 -38.91037 25.219683 0.0)*/

    // PUNTO P1  (53.60281 -11.933228 -16.063263 49.03339 -38.91037 25.219683 0.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -41.9859;
                leftArmQ[1] = -12.8998;
                leftArmQ[2] = 49.4727;
                leftArmQ[3] = -56.6607;
                leftArmQ[4] = -25.0263;
                leftArmQ[5] = 0.0;

                rightArmQ[0] = 53.60281;
                rightArmQ[1] = -11.933228;
                rightArmQ[2] = -16.063263;
                rightArmQ[3] = 49.03339;
                rightArmQ[4] = -38.91037;
                rightArmQ[5] = 25.219683;
                rightArmQ[6] = 1200;
                movingArmJoints(leftArmQ,rightArmQ);
                }

    // PUNTO P2  (57.469242 -12.108978 -41.54657 71.968369 -38.99826 53.427063 0.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -41.9859;
                leftArmQ[1] = -12.8998;
                leftArmQ[2] = 49.4727;
                leftArmQ[3] = -56.6607;
                leftArmQ[4] = -25.0263;
                leftArmQ[5] = 0.0;

                rightArmQ[0] = 57.469242;
                rightArmQ[1] = -12.108978;
                rightArmQ[2] = -41.54657;
                rightArmQ[3] = 71.968369;
                rightArmQ[4] = -38.99826;
                rightArmQ[5] = 53.427063;
                rightArmQ[6] = 1200;
                movingArmJoints(leftArmQ,rightArmQ);
                }

    // PUNTO P3  (57.820736 -5.606323 -44.797882 71.968369 -38.91037 45.782074 0.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -41.9859;
                leftArmQ[1] = -12.8998;
                leftArmQ[2] = 49.4727;
                leftArmQ[3] = -56.6607;
                leftArmQ[4] = -25.0263;
                leftArmQ[5] = 0.0;

                rightArmQ[0] = 57.820736;
                rightArmQ[1] = -5.606323;
                rightArmQ[2] = -44.797882;
                rightArmQ[3] = 71.968369;
                rightArmQ[4] = -38.91037;
                rightArmQ[5] = 45.782074;
                rightArmQ[6] = 1200;
                movingArmJoints(leftArmQ,rightArmQ);
                }

    // Punto P4:  (57.732864 0.790861 -44.797882 71.968369 -38.91037 38.927944 0.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -41.9859;
                leftArmQ[1] = -12.8998;
                leftArmQ[2] = 49.4727;
                leftArmQ[3] = -56.6607;
                leftArmQ[4] = -25.0263;
                leftArmQ[5] = 0.0;

                rightArmQ[0] = 57.732864;
                rightArmQ[1] = 0.790861;
                rightArmQ[2] = -44.797882;
                rightArmQ[3] = 71.968369;
                rightArmQ[4] = -38.91037;
                rightArmQ[5] = 38.927944;
                rightArmQ[6] = 1200;
                movingArmJoints(leftArmQ,rightArmQ);
                }

    // Punto PF:  (58.17223 0.175747 -55.782074 71.616875 -38.82251 28.20738 0.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -41.9859;
                leftArmQ[1] = -12.8998;
                leftArmQ[2] = 49.4727;
                leftArmQ[3] = -56.6607;
                leftArmQ[4] = -25.0263;
                leftArmQ[5] = 0.0;

                rightArmQ[0] = 58.17223;
                rightArmQ[1] = 0.175747;
                rightArmQ[2] = -55.782074;
                rightArmQ[3] = 71.616875;
                rightArmQ[4] = -38.82251;
                rightArmQ[5] = 28.20738;
                rightArmQ[6] = 1200;
                movingArmJoints(leftArmQ,rightArmQ);
                }

    // Close right hand

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -41.9859;
                leftArmQ[1] = -12.8998;
                leftArmQ[2] = 49.4727;
                leftArmQ[3] = -56.6607;
                leftArmQ[4] = -25.0263;
                leftArmQ[5] = 0.0;

                rightArmQ[0] = 58.17223;
                rightArmQ[1] = 0.175747;
                rightArmQ[2] = -55.782074;
                rightArmQ[3] = 71.616875;
                rightArmQ[4] = -38.82251;
                rightArmQ[5] = 28.20738;
                rightArmQ[6] = -1200;
                movingArmJoints(leftArmQ,rightArmQ);
                }

                yarp::os::Time::delay(1);
                break;
            }

            case VOCAB_SECOND_MOVEMENT: {//VOCAB_SECOND_MOVEMENT

                printf("giving water - 2nd part\n");

    // Punto P5:  (66.080841 5.975395 -26.871704 35.413006 -70.808441 3.163445 0.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -20;
                leftArmQ[1] = -10;
                leftArmQ[2] = 0;
                leftArmQ[3] = -60;
                leftArmQ[4] = 10;
                leftArmQ[5] = -10;

                rightArmQ[0] = 66.080841;
                rightArmQ[1] = 5.975395;
                rightArmQ[2] = -26.871704;
                rightArmQ[3] = 35.413006;
                rightArmQ[4] = -70.808441;
                rightArmQ[5] = 3.163445;
                rightArmQ[6] = -1200;
                movingArmJoints(leftArmQ,rightArmQ);
                }

                yarp::os::Time::delay(2);

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

                rightArmQ[0] = 66.080841;
                rightArmQ[1] = 5.975395;
                rightArmQ[2] = -26.871704;
                rightArmQ[3] = 35.413006;
                rightArmQ[4] = -70.808441;
                rightArmQ[5] = 3.163445;
                rightArmQ[6] = 1200;
                movingArmJoints(leftArmQ,rightArmQ);
                }

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

                rightArmQ[0] = 66.080841;
                rightArmQ[1] = 5.975395;
                rightArmQ[2] = -26.871704;
                rightArmQ[3] = 35.413006;
                rightArmQ[4] = -70.808441;
                rightArmQ[5] = 3.163445;
                rightArmQ[6] = 1200;
                movingArmJoints(leftArmQ,rightArmQ);
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

