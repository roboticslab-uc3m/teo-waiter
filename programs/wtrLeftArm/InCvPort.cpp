// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "InCvPort.hpp"

namespace teo
{

/************************************************************************/

void InCvPort::onRead(Bottle& b) {
    switch ( follow ) {
        case 1: {//VOCAB_HELLO_TEO

        printf("begin MOVE TO P1 POSITION\n");
        double P1[7] = {-20, -10, 0, -60, 10, -10, 0};
        iPositionControl->positionMove(P1);
        bool done1 = false;
        while( ! done1 )    {
            yarp::os::Time::delay(0.2);
            iPositionControl->checkMotionDone(&done1);
            printf(".");
            fflush(stdout);
        }

            /*iPositionControl->positionMove(0, -20);
            iPositionControl->positionMove(1, -10);
            iPositionControl->positionMove(2, 0);
            iPositionControl->positionMove(3, -60);
            iPositionControl->positionMove(4, 10);
            iPositionControl->positionMove(5, -10);
            iPositionControl->positionMove(6, 0);*/
            break;
        }
        case 2: {//VOCAB_GO_TEO

            int a=0;
            if (a==0 && follow) {
                printf("begin MOVE TO Pa POSITION\n");
                double Pa[7] = {-30, 40, 0, -70, -40, 10, 0};
                iPositionControl->positionMove(Pa);

                /*printf("MOVE TO POSITION 01\n");
                iPositionControl->positionMove(0, -30);
                iPositionControl->positionMove(1, 40);
                iPositionControl->positionMove(2, 0);
                iPositionControl->positionMove(3, -70);
                iPositionControl->positionMove(4, -40);
                iPositionControl->positionMove(5, 10);
                iPositionControl->positionMove(6, 0);*/

                Time::delay(3);
                a=1;
            }

            if (a==1 && follow) {
                printf("begin MOVE TO Pb POSITION\n");
                double Pb[7] = {-20, 30, 0, -80, -30, 10, 0};
                iPositionControl->positionMove(Pb);

                /*printf("MOVE TO POSITION 02\n");
                iPositionControl->positionMove(0, -20);
                iPositionControl->positionMove(1, 30);
                iPositionControl->positionMove(2, 0);
                iPositionControl->positionMove(3, -80);
                iPositionControl->positionMove(4, -30);
                iPositionControl->positionMove(5, 10);
                iPositionControl->positionMove(6, 0);*/

                Time::delay(3);
                a=2;
            }

            if (a==2 && follow) {
                printf("begin MOVE TO Pc POSITION\n");
                double Pc[7] = {-30, -10, 0, -70, 10, 10, 0};
                iPositionControl->positionMove(Pc);

                /*printf("MOVE TO POSITION 03\n");
                iPositionControl->positionMove(0, -30);
                iPositionControl->positionMove(1, -10);
                iPositionControl->positionMove(2, 0);
                iPositionControl->positionMove(3, -70);
                iPositionControl->positionMove(4, 10);
                iPositionControl->positionMove(5, 10);
                iPositionControl->positionMove(6, 0);*/

                Time::delay(3);
                a=0;
            }
            break;
        }
        case 3: {//VOCAB_WATER_PLEASE

            printf("begin MOVE TO P1 POSITION\n");
            double P1[7] = {-20, -10, 0, -60, 10, -10, 0};
            iPositionControl->positionMove(P1);
            bool done1 = false;
            while( ! done1 )    {
                yarp::os::Time::delay(0.2);
                iPositionControl->checkMotionDone(&done1);
                printf(".");
                fflush(stdout);
            }

            /*iPositionControl->positionMove(0, -40);
            iPositionControl->positionMove(1, -10);
            iPositionControl->positionMove(2, 55);
            iPositionControl->positionMove(3, -60);
            iPositionControl->positionMove(4, -25);
            iPositionControl->positionMove(5, 0);
            iPositionControl->positionMove(6, 0);*/
            break;
        }
        case 4: {//VOCAB_SECOND_MOVEMENT

            printf("begin MOVE TO P1 POSITION\n");
            double P1[7] = {-20, -10, 0, -60, 10, -10, 0};
            iPositionControl->positionMove(P1);
            bool done1 = false;
            while( ! done1 )    {
                yarp::os::Time::delay(0.2);
                iPositionControl->checkMotionDone(&done1);
                printf(".");
                fflush(stdout);
            }
            break;
        }
        case 5: {//VOCAB_STOP_TEO
            printf("begin MOVE TO Poff POSITION\n");
            double P1[7] = {0, 0, 0, 0, 0, 0, 0};
            iPositionControl->positionMove(P1);
            bool done1 = false;
            while( ! done1 )    {
                yarp::os::Time::delay(0.2);
                iPositionControl->checkMotionDone(&done1);
                printf(".");
                fflush(stdout);
            }

            /*iPositionControl->positionMove(0, 0);
            iPositionControl->positionMove(1, 0);
            iPositionControl->positionMove(2, 0);
            iPositionControl->positionMove(3, 0);
            iPositionControl->positionMove(4, 0);
            iPositionControl->positionMove(5, 0);
            iPositionControl->positionMove(6, 0);*/
            break;
        }
        default:
            break;
    }

}

/************************************************************************/

void InCvPort::setFollow(int value)
{
    follow = value;
}

/************************************************************************/

}  // namespace teo

