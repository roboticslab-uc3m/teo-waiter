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

    //state = VOCAB_STATE_ARM_SWINGING;

    std::string WtrArmsStr("/wtrArms");

    yarp::os::Property leftArmOptions;
    leftArmOptions.put("device","remote_controlboard");
    leftArmOptions.put("remote",robot+"/leftArm");
    leftArmOptions.put("local",WtrArmsStr+robot+"/leftArm");
    leftArmDevice.open(leftArmOptions);
    if(!leftArmDevice.isValid())    {
        printf("robot leftArm device not available.\n");
        leftArmDevice.close();
        yarp::os::Network::fini();
        return false;
    }

    if ( ! leftArmDevice.view(leftArmIPositionControl) )    {
        printf("[warning] Problems acquiring leftArmPos interface\n");
        return false;
    } else printf("[success] Acquired leftArmPos interface\n");
    leftArmIPositionControl->setPositionMode();

    yarp::os::Property rightArmOptions;
    rightArmOptions.put("device","remote_controlboard");
    rightArmOptions.put("remote",robot+"/rightArm");
    rightArmOptions.put("local",WtrArmsStr+robot+"/rightArm");
    rightArmDevice.open(rightArmOptions);
    if(!rightArmDevice.isValid())    {
        printf("robot rightArm device not available.\n");
        rightArmDevice.close();
        yarp::os::Network::fini();
      return false;
    }

    if ( ! rightArmDevice.view(rightArmIPositionControl) )    {
        printf("[warning] Problems acquiring rightArmPos interface\n");
        return false;
    } else printf("[success] Acquired rightArmPos interface\n");
    rightArmIPositionControl->setPositionMode();

    phase = false;

    inSrPort.open("/wtrArms/dialogue/command:i");
    inSrPort.setReader(*this);  //-- Callback reader: avoid need to call inSrPort.read().

    return this->start();  //-- Start the thread (calls run).
}

/************************************************************************/

bool WtrArms::interruptModule()
{
    this->stop();
    inSrPort.interrupt();
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
    rightArmIPositionControl->positionMove( rightArmQ.data() );
    leftArmIPositionControl->positionMove( leftArmQ.data() );
    //printf("Waiting for right arm.");

    bool doneR = false; // checking the position move
    while((!doneR)&&(!Thread::isStopping()))
    {
        rightArmIPositionControl->checkMotionDone(&doneR);
        //printf(".");
        //fflush(stdout);
        yarp::os::Time::delay(0.1);
    }

    bool doneL = false; // checking the position move
    while((!doneL)&&(!Thread::isStopping()))
    {
        leftArmIPositionControl->checkMotionDone(&doneL);
        //printf(".");
        //fflush(stdout);
        yarp::os::Time::delay(0.1);
    }
    //printf("\n");
    return true;
}

/************************************************************************/

bool WtrArms::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle b;
    b.read(connection);
    // process data in b
    printf("[WtrArms] Got %s\n", b.toString().c_str());

    /*if( (VOCAB_FOLLOW_ME ==b.get(0).asVocab()) || (VOCAB_STATE_SALUTE ==b.get(0).asVocab()) )
    {
     state = VOCAB_STATE_SALUTE;
    }
    else if (VOCAB_STOP_FOLLOWING ==b.get(0).asVocab())
     state = VOCAB_STOP_FOLLOWING;*/

    state = b.get(0).asVocab();

    return true;
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
                leftArmQ[1] = -10;
                leftArmQ[2] = 0;
                leftArmQ[3] = -60;
                leftArmQ[4] = 10;
                leftArmQ[5] = -10;
                movingArmJoints(leftArmQ,rightArmQ);
                }

                break;
            }

            case VOCAB_GO_TEO: {//VOCAB_GO_TEO

                printf("stability\n");
                int a=0;

                if (a==0 && state) {
                    printf("begin MOVE TO Pa POSITION\n");
                    double Pa[7] = {-30, 40, 0, -70, -40, 10, 0};
                    leftArmIPositionControl->positionMove(Pa);
                    yarp::os::Time::delay(4);
                    a=1;
                } // MOVIMIENTO NUMERO 1

                if (a==1 && state) {
                    printf("begin MOVE TO Pb POSITION\n");
                    double Pb[7] = {10, 30, 0, -95, -30, -5, 0};
                    leftArmIPositionControl->positionMove(Pb);
                    yarp::os::Time::delay(3);
                    a=2;
                } // MOVIMIENTO NUMERO 2

                if (a==2 && state) {
                    printf("begin MOVE TO Pc POSITION\n");
                    double Pc[7] = {-30, -10, 0, -70, 10, 10, 0};
                    leftArmIPositionControl->positionMove(Pc);
                    yarp::os::Time::delay(4);
                    a=0;
                } // MOVIMIENTO NUMERO 3

                break;
            }

            case VOCAB_WATER_PLEASE: {//VOCAB_WATER_PLEASE

                printf("giving water - 1st part\n");

              /*// puntos de aproximacion
                // Punto P5:  (66.080841 5.975395 -26.871704 35.413006 -70.808441 3.163445 0.0) [tsta] 10 1481555264.023854 [ok]
                // Punto PF:  (58.17223 0.175747 -55.782074 71.616875 -38.82251 28.20738 0.0) [tsta] 13 1481551585.858203 [ok]
                // Punto P4:  (57.732864 0.790861 -44.797882 71.968369 -38.91037 38.927944 0.0) [tsta] 9 1481553507.348431 [ok]
                // PUNTO P3  (57.820736 -5.606323 -44.797882 71.968369 -38.91037 45.782074 0.0) [tsta] 10 1481553695.392905 [ok]
                // PUNTO P2  (57.469242 -12.108978 -41.54657 71.968369 -38.99826 53.427063 0.0) [tsta] 11 1481553769.980141 [ok]
                // PUNTO P1  (53.60281 -11.933228 -16.063263 49.03339 -38.91037 25.219683 0.0)*/

    // PUNTO P1  (53.60281 -11.933228 -16.063263 49.03339 -38.91037 25.219683 0.0)

                {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[0] = -40;
                leftArmQ[1] = -10;
                leftArmQ[2] = 55;
                leftArmQ[3] = -60;
                leftArmQ[4] = -25;
                leftArmQ[5] = 0;

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
                leftArmQ[0] = -40;
                leftArmQ[1] = -10;
                leftArmQ[2] = 55;
                leftArmQ[3] = -60;
                leftArmQ[4] = -25;
                leftArmQ[5] = 0;

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
                leftArmQ[0] = -40;
                leftArmQ[1] = -10;
                leftArmQ[2] = 55;
                leftArmQ[3] = -60;
                leftArmQ[4] = -25;
                leftArmQ[5] = 0;

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
                leftArmQ[0] = -40;
                leftArmQ[1] = -10;
                leftArmQ[2] = 55;
                leftArmQ[3] = -60;
                leftArmQ[4] = -25;
                leftArmQ[5] = 0;

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
                leftArmQ[0] = -40;
                leftArmQ[1] = -10;
                leftArmQ[2] = 55;
                leftArmQ[3] = -60;
                leftArmQ[4] = -25;
                leftArmQ[5] = 0;

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
                leftArmQ[0] = -40;
                leftArmQ[1] = -10;
                leftArmQ[2] = 55;
                leftArmQ[3] = -60;
                leftArmQ[4] = -25;
                leftArmQ[5] = 0;

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

