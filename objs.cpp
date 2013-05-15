#include <gl/gl.h>     // The GL Header File
#include <GL/glu.h>
#include <gl/glut.h>
#include "objs.h"
#include "BallMath.h"

#define LG_NSEGS 4
#define NSEGS (1<<LG_NSEGS)

HMatrix mId = { 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 };

static GLUquadricObj * myQuad;

void DrawAnyArc(HVect vFrom, HVect vTo);

/* Establish reasonable initial values for controller. */
void Joint::Ball_Init()
{
    int i;
    center = qOne;
    vDown = vNow = qOne;
    qDown = qNow = qOne;
    for (i=15; i>=0; i--)
	mNow[i] = mDown[i] = mId[i];
    dragging = GL_FALSE;
    myQuad = gluNewQuadric();
    gluQuadricDrawStyle(myQuad, GLU_SILHOUETTE);
}

Joint::Joint(double r){
	radius = r; angleX=0; angleY=0; angleZ=0;
	minX = 0;
		maxX = 0;
		minY = 0;
		maxY = 0;
		minZ = 0;
		maxZ = 0;
		Ball_Init();
}

Joint::Joint(double r, int min_x, int max_x, int min_y, int max_y, int min_z, int max_z){
	radius = r; angleX=0; angleY=0; angleZ=0;
	minX = min_x;
		maxX = max_x;
		minY = min_y;
		maxY = max_y;
		minZ = min_z;
		maxZ = max_z;
		Ball_Init();
}

/* Set the center and size of the controller. */
void Joint::Ball_Place(HVect c)
{
    center = c;
}

/* Incorporate new mouse position. */
void Joint::Ball_Mouse(HVect v)
{
    vNow = v;
}

/* Using vDown, vNow, dragging, compute rotation etc. */
void Joint::Ball_Update()
{
    int i;

    vFrom = MouseOnSphere(vDown, center, radius);
    vTo = MouseOnSphere(vNow, center, radius);
    if (dragging) {
	qDrag = Qt_FromBallPoints(vFrom, vTo);
	qNow = Qt_Mul(qDrag, qDown);
    }
    Qt_ToBallPoints(qDown, &vrFrom, &vrTo);
    Qt_ToMatrix(qNow, mNow);
}

/* Return rotation matrix defined by controller use. */
void Joint::Ball_Value(HMatrix m)
{
    int i;
    for (i=15; i>=0; i--)
    	m[i] = mNow[i];
}


/* Begin drag sequence. */
void Joint::Ball_BeginDrag()
{
    dragging = GL_TRUE;
    vDown = vNow;
    vTo   = vFrom;
}

/* Stop drag sequence. */
void Joint::Ball_EndDrag()
{
    int i;
   dragging = GL_FALSE;
    qDown = qNow;
    for (i=15; i>=0; i--)
	mDown[i] = mNow[i];
}

/* Draw an arc defined by its ends. */
void DrawAnyArc(HVect vFrom, HVect vTo)
{
    int i;
    HVect pts[NSEGS+1];
    double dot;

    pts[0] = vFrom;
    pts[1] = pts[NSEGS] = vTo;
    for (i=0; i<LG_NSEGS; i++)
    	pts[1] = V3_Bisect(pts[0], pts[1]);
    dot = 2.0*V3_Dot(pts[0], pts[1]);
    for (i=2; i<NSEGS; i++) {
	pts[i] = V3_Sub(V3_Scale(pts[i-1], dot), pts[i-2]);
    }
    glBegin(GL_LINE_STRIP);
    for (i=0; i<=NSEGS; i++)
	glVertex3fv((float *)&pts[i]);
    glEnd();
}

/* Draw "rubber band" arc during dragging. */
void Joint::Ball_DrawDragArc()
{
    if (dragging)
      DrawAnyArc(vFrom, vTo);
}

void Joint::draw(HMatrix Rot)
{
	float r = radius;
	glutSolidSphere(radius,10,10);
	/**     glPushMatrix();
	      glColor3f(1.0, 1.0, 1.0);
	      glScalef(r, r, r);
	      gluDisk(myQuad, 0.0, 1.0, 24, 1);
	      Ball_DrawDragArc();
	 glPopMatrix(); */
}

void Joint::setRotBounds(int min_x, int max_x, int min_y, int max_y, int min_z, int max_z)
{
	minX = min_x;
	maxX = max_x;
	minY = min_y;
	maxY = max_y;
	minZ = min_z;
	maxZ = max_z;
}

void Joint::rotateX(int i)
{
	int newAngle = (angleX+i)%360;
if(minX==0 && maxX==0) angleX = newAngle;
else if(newAngle>minX && newAngle<maxX)
	angleX = newAngle;
else {}
}

void Joint::rotateY(int j)
{
int newAngle = (angleY+j)%360;
if(minY==0 && maxY==0) angleY = newAngle;
else if(newAngle>minY && newAngle<maxY)
	angleY = newAngle;
else {}}

void Joint::rotateZ(int k)
{int newAngle = (angleZ+k)%360;
if(minZ==0 && maxZ==0) angleZ = newAngle;
else if(newAngle>minZ && newAngle<maxZ)
	angleZ = newAngle;
else {}
}
