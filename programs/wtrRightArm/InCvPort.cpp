// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "InCvPort.hpp"

namespace teo
{

/************************************************************************/

void InCvPort::onRead(Bottle& b) {

    if ( ! follow )    {
        double initpos[7] = {0,0,0,0,0,0,0};
        iPositionControl->positionMove(initpos);
    return;
    }

    // Punto P5: Response: [is] encs (66.080841 5.975395 -26.871704 35.413006 -70.808441 3.163445 0.0) [tsta] 10 1481555264.023854 [ok]
    // Punto PF: Response: [is] encs (58.17223 0.175747 -55.782074 71.616875 -38.82251 28.20738 0.0) [tsta] 13 1481551585.858203 [ok]
    // Punto P4: Response: [is] encs (57.732864 0.790861 -44.797882 71.968369 -38.91037 38.927944 0.0) [tsta] 9 1481553507.348431 [ok]
    // PUNTO P3 Response: [is] encs (57.820736 -5.606323 -44.797882 71.968369 -38.91037 45.782074 0.0) [tsta] 10 1481553695.392905 [ok]
    // PUNTO P2 Response: [is] encs (57.469242 -12.108978 -41.54657 71.968369 -38.99826 53.427063 0.0) [tsta] 11 1481553769.980141 [ok]
    // PUNTO P1 Response: [is] encs (53.60281 -11.933228 -16.063263 49.03339 -38.91037 25.219683 0.0) [tsta] 12 1481553834.776767 [ok]


    // PUNTO P2: Response: [is] encs (59.841827 -10.966614 -50.070312 71.616875 -38.82251 46.397186 0.0) [tsta] 14 1481551699.912557 [ok]
    // PUNTO P1: Response: [is] encs (57.029877 -10.878723 -15.887512 53.427063 -38.82251 29.525482 0.0) [tsta] 15 1481551772.422007 [ok]



    printf("begin MOVE TO P1 POSITION\n");
    double P1[7] = {53.60281, -11.933228, -16.063263, 49.03339, -38.91037, 25.219683, 0.0};
    iPositionControl->positionMove(P1);
    bool done1 = false;
    while( ! done1 )    {
        yarp::os::Time::delay(0.2);
        iPositionControl->checkMotionDone(&done1);
        printf(".");
        fflush(stdout);
    }


    printf("begin MOVE TO P2 POSITION\n");
    double P2[7] = {57.469242, -12.108978, -41.54657, 71.968369, -38.99826, 53.427063, 0.0};
    iPositionControl->positionMove(P2);
    bool done2 = false;
    while( ! done2 )    {
        yarp::os::Time::delay(0.2);
        iPositionControl->checkMotionDone(&done2);
        printf(".");
        fflush(stdout);
    }


    printf("begin MOVE TO P3 POSITION\n");
    double P3[7] = {57.820736, -5.606323, -44.797882, 71.968369, -38.91037, 45.782074, 0.0};
    iPositionControl->positionMove(P3);
    bool done3 = false;
    while( ! done3 )    {
        yarp::os::Time::delay(0.2);
        iPositionControl->checkMotionDone(&done3);
        printf(".");
        fflush(stdout);
    }


    printf("begin MOVE TO P4 POSITION\n");
    double P4[7] = {57.732864, 0.790861, -44.797882, 71.968369, -38.91037, 38.927944, 0.0};
    iPositionControl->positionMove(P4);
    bool done41 = false;
    while( ! done41 )    {
        yarp::os::Time::delay(0.2);
        iPositionControl->checkMotionDone(&done41);
        printf(".");
        fflush(stdout);
    }


    printf("begin MOVE TO PF POSITION\n");
    double PF[7] = {58.17223, 0.175747, -55.782074, 71.616875, -38.82251, 28.20738, 0.0};
    iPositionControl->positionMove(PF);
    bool doneF = false;
    while( ! doneF )    {
        yarp::os::Time::delay(0.2);
        iPositionControl->checkMotionDone(&doneF);
        printf(".");
        fflush(stdout);
    }


    printf("begin MOVE TO P4 POSITION\n");
//    double P4[7] = {57.732864, 0.790861, -44.797882, 71.968369, -38.91037, 38.927944, 0.0};
    iPositionControl->positionMove(P4);
    bool done42 = false;
    while( ! done42 )    {
        yarp::os::Time::delay(0.2);
        iPositionControl->checkMotionDone(&done42);
        printf(".");
        fflush(stdout);
    }


    printf("begin MOVE TO P5 POSITION\n");
    double P5[7] = {66.080841, 5.975395, -26.871704, 35.413006, -70.808441, 3.163445, 0.0};
    iPositionControl->positionMove(P5);
    bool done5 = false;
    while( ! done5 )    {
        yarp::os::Time::delay(0.2);
        iPositionControl->checkMotionDone(&done5);
        printf(".");
        fflush(stdout);
    }


    /*
    switch ( follow ) {
        case 1: {//VOCAB_HELLO_TEO

            iPositionControl->positionMove(0, -30);
            iPositionControl->positionMove(1, -10);
            iPositionControl->positionMove(2, 0);
            iPositionControl->positionMove(3, -70);
            iPositionControl->positionMove(4, 10);
            iPositionControl->positionMove(5, 10);
            iPositionControl->positionMove(6, 0);
            break;
        }
        case 2: {//VOCAB_GO_TEO

            int a=0;
            if (a==0 && follow) {
                printf("MOVE TO POSITION 01\n");
                iPositionControl->positionMove(0, -30);
                iPositionControl->positionMove(1, 40);
                iPositionControl->positionMove(2, 0);
                iPositionControl->positionMove(3, -70);
                iPositionControl->positionMove(4, -40);
                iPositionControl->positionMove(5, 10);
                iPositionControl->positionMove(6, 0);

                Time::delay(3);
                a=1;
            }

            if (a==1 && follow) {
                printf("MOVE TO POSITION 02\n");
                iPositionControl->positionMove(0, -10);
                iPositionControl->positionMove(1, 30);
                iPositionControl->positionMove(2, 0);
                iPositionControl->positionMove(3, -90);
                iPositionControl->positionMove(4, -30);
                iPositionControl->positionMove(5, 10);
                iPositionControl->positionMove(6, 0);

                Time::delay(3);
                a=2;
            }

            if (a==2 && follow) {
                printf("MOVE TO POSITION 03\n");
                iPositionControl->positionMove(0, -30);
                iPositionControl->positionMove(1, -10);
                iPositionControl->positionMove(2, 0);
                iPositionControl->positionMove(3, -70);
                iPositionControl->positionMove(4, 10);
                iPositionControl->positionMove(5, 10);
                iPositionControl->positionMove(6, 0);

                Time::delay(3);
                a=0;
            }
            break;
        }
        case 3: {//VOCAB_WATER_PLEASE

        int a=0;
        if (a==0 && follow) {
            printf("MOVE TO POSITION 01\n");
            iPositionControl->positionMove(0, -30);
            iPositionControl->positionMove(1, 40);
            iPositionControl->positionMove(2, 0);
            iPositionControl->positionMove(3, -70);
            iPositionControl->positionMove(4, -40);
            iPositionControl->positionMove(5, 10);
            iPositionControl->positionMove(6, 0);

            Time::delay(3);
            a=1;
        }

        if (a==1 && follow) {
            printf("MOVE TO POSITION 02\n");
            iPositionControl->positionMove(0, -10);
            iPositionControl->positionMove(1, 30);
            iPositionControl->positionMove(2, 0);
            iPositionControl->positionMove(3, -90);
            iPositionControl->positionMove(4, -30);
            iPositionControl->positionMove(5, 10);
            iPositionControl->positionMove(6, 0);

            Time::delay(3);
            a=2;
        }

        if (a==2 && follow) {
            printf("MOVE TO POSITION 03\n");
            iPositionControl->positionMove(0, -30);
            iPositionControl->positionMove(1, -10);
            iPositionControl->positionMove(2, 0);
            iPositionControl->positionMove(3, -70);
            iPositionControl->positionMove(4, 10);
            iPositionControl->positionMove(5, 10);
            iPositionControl->positionMove(6, 0);

            Time::delay(3);
            a=0;
        }
        }
        case 4: {//VOCAB_STOP_TEO
            iPositionControl->positionMove(0, 0);
            iPositionControl->positionMove(1, 0);
            iPositionControl->positionMove(2, 0);
            iPositionControl->positionMove(3, 0);
            iPositionControl->positionMove(4, 0);
            iPositionControl->positionMove(5, 0);
            iPositionControl->positionMove(6, 0);
            break;
        }
        default:
            break;
    }*/

}

/************************************************************************/

void InCvPort::setFollow(int value)
{
    follow = value;
}

/************************************************************************/

}  // namespace teo

