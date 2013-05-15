/***** BallMath.h - Essential routines for Arcball.  *****/
#ifndef _H_BallMath
#define _H_BallMath
#include "BallAux.h"

extern HVect MouseOnSphere(HVect mouse, HVect ballCenter, double ballRadius);
extern HVect ConstrainToAxis(HVect loose, HVect axis);
extern int NearestConstraintAxis(HVect loose, HVect *axes, int nAxes);
extern Quat Qt_FromBallPoints(HVect from, HVect to);
extern void Qt_ToBallPoints(Quat q, HVect *arcFrom, HVect *arcTo);

#endif
