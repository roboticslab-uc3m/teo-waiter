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
        printf("\t--robot: %s [%s]\n",robot.c_str(),DEFAULT_ROBOT);
        ::exit(0);
    }

    state = VOCAB_STATE_ARM_SWINGING;

    std::string WtrArmsStr("/wtrArms");

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

    if ( ! leftArmDevice.view(leftArmIPositionControl) ) {
        printf("[warning] Problems acquiring leftArmPos interface\n");
        return false;
    } else printf("[success] Acquired leftArmPos interface\n");
    leftArmIPositionControl->setPositionMode();

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

    if ( ! rightArmDevice.view(rightArmIPositionControl) ) {
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
    bool done = false;
    while((!done)&&(!Thread::isStopping()))
    {
        rightArmIPositionControl->checkMotionDone(&done);
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
     printf("[FollowMeArmSwing] Got %s\n", b.toString().c_str());
     if( (VOCAB_FOLLOW_ME ==b.get(0).asVocab()) || (VOCAB_STATE_SALUTE ==b.get(0).asVocab()) )
     {
         state = VOCAB_STATE_SALUTE;
     }
     else if (VOCAB_STOP_FOLLOWING ==b.get(0).asVocab())
         state = VOCAB_STOP_FOLLOWING;

     return true;
}

/************************************************************************/

void WtrArms::run()
{
    while( !Thread::isStopping() )
    {
        switch (state)
        {
        case VOCAB_STATE_ARM_SWINGING:
            if(phase)
            {
                printf("Phase: true\n");
                std::vector<double> leftArmQ(7,0.0);
                leftArmQ[0] = 20;
                leftArmQ[1] = 5;
                std::vector<double> rightArmQ(7,0.0);
                rightArmQ[0] = 20;
                movingArmJoints(leftArmQ,rightArmQ);
                phase = false;
            }
            else
            {
                printf("Phase: false\n");
                std::vector<double> leftArmQ(7,0.0);
                leftArmQ[0] = -20;
                leftArmQ[1] = 5;
                std::vector<double> rightArmQ(7,0.0);
                rightArmQ[0] = -20;
                movingArmJoints(leftArmQ,rightArmQ);
                phase = true;
            }
            break;

        case VOCAB_STATE_SALUTE:
            printf("Salute\n");
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                rightArmQ[0] = 45;
                rightArmQ[2] = 20;
                rightArmQ[3] = 80;
                movingArmJoints(leftArmQ,rightArmQ);
            }
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                rightArmQ[0] = 45;
                rightArmQ[2] = -20;
                rightArmQ[3] = 80;
                movingArmJoints(leftArmQ,rightArmQ);
            }
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                rightArmQ[0] = 45;
                rightArmQ[2] = 20;
                rightArmQ[3] = 80;
                movingArmJoints(leftArmQ,rightArmQ);
            }
            state = VOCAB_STATE_ARM_SWINGING;
            break;

        case VOCAB_STOP_FOLLOWING:
            printf("Stop Following\n");
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);

                movingArmJoints(leftArmQ,rightArmQ);
            }
            break;

        default:
            printf("Bad state!\n");
            break;
        }
    }

    return;
}

/************************************************************************/

}  // namespace teo

