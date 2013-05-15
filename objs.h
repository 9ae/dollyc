#ifndef OBJS_H_
#define OBJS_H_

#include <cmath> // for sqrt
#include <GL\gl.h>
#include <GL\glu.h>
#include "BallAux.h"
#include "BallMath.h"

using namespace std;

class Joint {
private:
	int angleX;
	int angleY;
	int angleZ;
	int minX;
	int minY;
	int minZ;
	int maxX;
	int maxY;
	int maxZ;
	HVect center;
	double radius;
	Quat qNow, qDown, qDrag;
	    HVect vNow, vDown, vFrom, vTo, vrFrom, vrTo;
	    HMatrix mNow, mDown;
	    Bool showResult, dragging;

public:

	Joint(double r);
	Joint(double r, int min_x, int max_x, int min_y, int max_y, int min_z, int max_z);
	~Joint() {};
	void draw(HMatrix Rot) ;
	int rotX() {return angleX;} ;
	int rotY() {return angleY;} ;
	int rotZ() {return angleZ;} ;
	void rotX(int n) {angleX=n;} ;
	void rotY(int n) {angleY=n;} ;
	void rotZ(int n) {angleZ=n;} ;
	void setRotBounds(int min_x, int max_x, int min_y, int max_y, int min_z, int max_z);
	void rotateX(int i);
	void rotateY(int j);
	void rotateZ(int k);
	void Ball_Init();
	void Ball_Place(HVect center);
	void Ball_Mouse(HVect vNow);
	void Ball_Update();
	void Ball_Value(HMatrix mNow);
	void Ball_BeginDrag();
	void Ball_EndDrag();
	void Ball_DrawDragArc();
};

#endif
