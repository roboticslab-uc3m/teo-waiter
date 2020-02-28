// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "InCvPort.hpp"

namespace teo
{

/************************************************************************/

void InCvPort::onRead(Bottle& b) {

    double now = yarp::os::Time::now();

    if (lastTimestamp != 0.0 && now - lastTimestamp < 0.1)
    {
        return;
    }

    lastTimestamp = now;

    //-------------------POSITION CONTROL MODE FOR HEAD--------------------

    if ( ! follow ){
    headIPositionControl->positionMove(0, 0.0);
    headIPositionControl->positionMove(1, 0.0);
    return;
    }
    if (b.size() < 4) return;

    double x = b.get(0).asDouble();
    double y = b.get(1).asDouble();
    //double z = b.get(2).asDouble();

    printf("%f %f\n",x,y);

    if( x > 320 ) headIPositionControl->relativeMove(0, -1);
    if( x < 300 ) headIPositionControl->relativeMove(0, 1);

    if( y > 290 ) headIPositionControl->relativeMove(1, 1);
    if( y < 270 ) headIPositionControl->relativeMove(1, -1);


    /*//------VELOCITY CONTROL MODE FOR HEAD------// no borrar
    if ( ! follow ){
	iVelocityControl->velocityMove(0, 0.0);
	iVelocityControl->velocityMove(1, 0.0); 
	return;
    }
    if (b.size() < 3) return;

    double x = b.get(0).asDouble();
    double y = b.get(1).asDouble();
    double z = b.get(2).asDouble();


    printf("%f %f %f\n",x,y,z);

    if( x > 215 ) {
        iVelocityControl->velocityMove(0, 6.0); //Id motor. Velocity [deg/s].
    }
    else if( x > 195 && x < 215 ) {
        iVelocityControl->velocityMove(0, 4.0);
    }
    else if( x > 175 && x < 195 ) {
        iVelocityControl->velocityMove(0, 2.0);
    }
    else if( x > 155 && x < 175 ) {
        iVelocityControl->velocityMove(0, 1.0);
    }
    else if( x > 145 && x < 155 ) {
        iVelocityControl->velocityMove(0, 0.0);
    }
    else if( x < 145 && x > 125 ) {
        iVelocityControl->velocityMove(0, -1.0);
    }
    else if( x < 125 && x > 105 ) {
        iVelocityControl->velocityMove(0, -2.0);
    }
    else if( x < 105 && x > 85 ) {
        iVelocityControl->velocityMove(0, -4.0);
    }
    else if( x < 85 ) {
        iVelocityControl->velocityMove(0, -6.0);
    }



    if( y > 185 ) {
        iVelocityControl->velocityMove(1, 5.0); //Id motor. Velocity [deg/s].
    }
    else if( y > 165 && x < 185) {
        iVelocityControl->velocityMove(1, 3.0);
    }
    else if( y > 145 && x < 165 ) {
        iVelocityControl->velocityMove(1, 1.0);
    }
    else if( y > 135 && x < 145 ) {
        iVelocityControl->velocityMove(1, 0.0);
    }
    else if( y < 135 && x > 115 ) {
        iVelocityControl->velocityMove(1, -1.0);
    }
    else if( y < 115 && x > 95 ) {
        iVelocityControl->velocityMove(1, -3.0);
    }
    else if( y < 95 ) {
        iVelocityControl->velocityMove(1, -5.0);
    }
*/
}

/************************************************************************/

void InCvPort::setFollow(bool value)
{
    follow = value;
}

/************************************************************************/

}  // namespace teo

