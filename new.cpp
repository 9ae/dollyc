#include <windows.h>   // Standard Header For Most Programs
#include <gl/gl.h>     // The GL Header File
#include <GL/glu.h>
#include <gl/glut.h>   // The GL Utility Toolkit (Glut) Header
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <cstdlib>
#include <cmath>
#include "Arm.h"
#include "Calf.h"
#include "Foot.h"
#include "Forearm.h"
#include "Hand.h"
#include "Head.h"
#include "Hip.h"
#include "Thigh.h"
#include "Torso.h"
#include "objs.h"
#include "BallAux.h"

// define limbs enum
#define HEAD 10
#define TORSO 11
#define HIP 12
#define RARM 13
#define RFOREARM 14
#define RHAND 15
#define RTHIGH 16
#define RCALF 17
#define RFOOT 18
#define LARM 19
#define LFOREARM 21
#define LHAND 22
#define LTHIGH 23
#define LCALF 24
#define LFOOT 25

// define mouse focus
#define FIGURE 100
#define LIGHT 101
typedef struct {long x, y;} Place;

Place winsize, winorig;
Place mouseNow, mouseDown;
HVect vNow;

float wX = 0, wY = 0, wZ = 0;
float cam = -4.0;
float moveX = 0.0, moveY = 1.0;
int active = 0;
int rMode = 0; // rotate mode 0-z, 1-x,2-y
Joint neck(0.2, -60, 90, -45, 45, -80, 80),
pelvis(0.44, -45, 90, -30, 30, -45, 45), body(0.1), rShoulder(0.2, -80, 30, -100, 100, -90, 90), lShoulder(
		0.2, -80, 30, -100, 100, -90, 90),
		rElbow(0.2, -1, 1, -150, 1, -1, 120), lElbow(0.2, -1, 1, -1, 150, -1,
				120), rWrist(0.2, -180, 30, -45, 45, -100, 90), lWrist(0.2,
				-180, 30, -45, 45, -100, 90), rLeg(0.28, -160, 90, -30, 30,
				-10, 90), lLeg(0.28, -160, 90, -30, 30, -90, 10), rKnee(0.2,
				-1, 170, -5, 10, -1, 1), lKnee(0.2, -1, 170, -10, 5, -1, 1),
		rAnkle(0.2, -20, 45, -15, 45, -1, 1), lAnkle(0.2, -20, 45, -55, 15, -1,1);
Joint *act = &body;
GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[] = { 0.88f, 0.44f, 0.88f, 0.0f };
GLfloat LightPosition[] = { -2.0f, 2.0f, 5.0f, 1.0f };
GLfloat color[3] = { 0.79, 0.7, 0.38 };                                           //*NEW* Current mouse point
int mouseFocus = FIGURE;
int oldX=-1, oldY=-1, olcZ=-1;
bool movexy=false,movez=false;

const float PI2 = 2.0*3.1415926535f;								// PI Squared


void gl_select(int x, int y);
void gl_selall(GLint hits, GLuint *buff);
void mouseClick();
void mousedw(int x, int y, int but);
void list_hits(GLint hits, GLuint *names);
void limbJoint();
void drawJoint(Joint j);
void drawLimb(int i);
void rotateJoint(signed int i);
void nullConstraints();
void jointLimb();

float distance(int x1, int x2, int y1, int y2){
	return sqrt(pow((float)y2-(float)y1,2)+pow((float)x2-(float)x1,2));
}

void captureViewport(){
	unsigned char *outputImage = 0;
	outputImage = new unsigned char[winsize.x * winsize.y * 3];
	glReadPixels(0, 0, winsize.x-1, winsize.y-1, GL_RGB,GL_UNSIGNED_BYTE, outputImage);
}

void drawJoint(Joint *j) {
	HMatrix mNow;

	j->Ball_Value(mNow);
	glMultMatrixf(mNow);
	if (act == j)
		glColor3f(0.8f, 0.0f, 0.0f);
	j->draw(mNow);
	glColor3f(color[0], color[1], color[2]);
}

void drawLimb(int i, float vert[], float norm[]) {
	GLsizei size = headNumVerts;
	if (i == HEAD) {
		vert = headVerts;
		norm = headNormals;
		size = headNumVerts;
	} else if (i == TORSO) {
		vert = torsoVerts;
		norm = torsoNormals;
		size = torsoNumVerts;
	} else if (i == HIP) {
		vert = hipVerts;
		norm = hipNormals;
		size = hipNumVerts;
	} else if (i == RARM || i == LARM) {
		vert = armVerts;
		norm = armNormals;
		size = armNumVerts;
	} else if (i == RFOREARM || i == LFOREARM) {
		vert = forearmVerts;
		norm = forearmNormals;
		size = forearmNumVerts;
	} else if (i == RHAND || i == LHAND) {
		vert = handVerts;
		norm = handNormals;
		size = handNumVerts;
	} else if (i == RTHIGH || i == LTHIGH) {
		vert = thighVerts;
		norm = thighNormals;
		size = thighNumVerts;
	} else if (i == RCALF || i == LCALF) {
		vert = calfVerts;
		norm = calfNormals;
		size = calfNumVerts;
	} else if (i == RFOOT || i == LFOOT) {
		vert = footVerts;
		norm = footNormals;
		size = footNumVerts;
	} else {
		return;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vert);
	glNormalPointer(GL_FLOAT, 0, norm);
	if (active == i)
		glColor3f(0.8f, 0.0f, 0.0f);
	glLoadName(i);
	glDrawArrays(GL_TRIANGLES, 0, size);
	glColor3f(color[0], color[1], color[2]);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void limbJoint() {
	switch (active) {
	case HEAD:
		act = &neck;
		break;
	case TORSO:
		act = &pelvis;
		break;

	case RARM:
		act = &rShoulder;
		break;
	case LARM:
		act = &lShoulder;
		break;

	case RFOREARM:
		act = &rElbow;
		break;
	case LFOREARM:
		act = &lElbow;
		break;

	case RHAND:
		act = &rWrist;
		break;
	case LHAND:
		act = &lWrist;
		break;

	case RTHIGH:
		act = &rLeg;
		break;
	case LTHIGH:
		act = &lLeg;
		break;

	case RCALF:
		act = &rKnee;
		break;
	case LCALF:
		act = &lKnee;
		break;

	case RFOOT:
		act = &rAnkle;
		break;
	case LFOOT:
		act = &lAnkle;
		break;
	default:
		act = &body;
		break;
	}
}

void init(GLvoid) // Create Some Everyday Functions
{

	glShadeModel(GL_SMOOTH); // Enable Smooth Shading
	glClearColor(0.05f, 0.56f, 0.77f, 0.88f); // Black Background
	glClearDepth(1.0f); // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST); // Enables Depth Testing
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
	glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);

}

void nullConstraints() {
	neck.setRotBounds(0, 0, 0, 0, 0, 0);
	rShoulder.setRotBounds(0, 0, 0, 0, 0, 0);
	lShoulder.setRotBounds(0, 0, 0, 0, 0, 0);
	rElbow.setRotBounds(0, 0, 0, 0, 0, 0);
	lElbow.setRotBounds(0, 0, 0, 0, 0, 0);
	rWrist.setRotBounds(0, 0, 0, 0, 0, 0);
	lWrist.setRotBounds(0, 0, 0, 0, 0, 0);
	rLeg.setRotBounds(0, 0, 0, 0, 0, 0);
	lLeg.setRotBounds(0, 0, 0, 0, 0, 0);
	rKnee.setRotBounds(0, 0, 0, 0, 0, 0);
	lKnee.setRotBounds(0, 0, 0, 0, 0, 0);
	rAnkle.setRotBounds(0, 0, 0, 0, 0, 0);
	lAnkle.setRotBounds(0, 0, 0, 0, 0, 0);
}

void newLightPost(int x, int y, int z){
	GLfloat AR = (GLfloat) winsize.x / (GLfloat) winsize.y;
	LightPosition[0] = (GLfloat)x*AR;
	LightPosition[1] = (GLfloat)y*AR;
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);
}

void newLightColor(int r, int g, int b){
	//Assume range 0 -255

	GLfloat newcolor[] = { (GLfloat)((float)r/255.0),
			(GLfloat)((float)g/255.0),
			(GLfloat)((float)b/255.0), 1.0f };

	glLightfv(GL_LIGHT1, GL_DIFFUSE, newcolor);

}

void changeLightPower(float i){
	GLfloat x = LightAmbient[1];
	if((x-i)>0.0 && (x+i)<1.0)
		x+= i;
	for(int j=0;j<3;j++)
		LightAmbient[j] = x;
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glEnable(GL_LIGHT1);
		glEnable(GL_LIGHTING);
}

void display(void) // Create The Display Function
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
	glLoadIdentity(); // Reset The Current Modelview Matrix

	glPushMatrix();
	glTranslatef(moveX, moveY, cam);
	glRotatef(wY, 0.0, 1.0, 0.0);
	glScalef(0.5, 0.5, 0.5);
	// Set position of pelvis bone
	drawJoint(&body);
	glRotatef((GLfloat) body.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) body.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) body.rotZ(), 0.0, 0.0, 1.0);
		glPushMatrix();
		glTranslatef(0.0, -0.9, 0.0);
		drawJoint(&pelvis);
		glRotatef((GLfloat) pelvis.rotX(), 1.0, 0.0, 0.0);
		glRotatef((GLfloat) pelvis.rotY(), 0.0, 1.0, 0.0);
		glRotatef((GLfloat) pelvis.rotZ(), 0.0, 0.0, 1.0);
		// Set position of torso
		glTranslatef(0.0, 0.9, 0.0);
			glPushMatrix();
			glTranslatef(0.0, -0.3, 0.0);
			glRotatef(-10.0, 1.0, 0.0, 0.0);
			glScalef(1.4, 1.4, 1.4);
			drawLimb(TORSO, NULL, NULL);
			glPopMatrix();

			glPushMatrix();
			// Move in position for neck bone
			glTranslatef(0.0, 0.5, 0.0);
			drawJoint(&neck);
			glRotatef((GLfloat) neck.rotX(), 1.0, 0.0, 0.0);
			glRotatef((GLfloat) neck.rotY(), 0.0, 1.0, 0.0);
			glRotatef((GLfloat) neck.rotZ(), 0.0, 0.0, 1.0);
			//Set position for head
			glTranslatef(0.0, 0.7, 0.0);
				glPushMatrix();
				drawLimb(HEAD, NULL, NULL);
				glPopMatrix();
			glPopMatrix();

			glPushMatrix();
			//Move position for R shoulder bone
			glTranslatef(0.5, 0.3, 0.0);
			drawJoint(&rShoulder);
			glRotatef((GLfloat) rShoulder.rotX(), 1.0, 0.0, 0.0);
			glRotatef((GLfloat) rShoulder.rotY(), 0.0, 1.0, 0.0);
			glRotatef((GLfloat) rShoulder.rotZ(), 0.0, 0.0, 1.0);
			// Set position for Right Arm
			glTranslatef(0.4, -0.2, 0.0);
				glPushMatrix();
				glScalef(1.0, 1.0, 1.0);
				glRotatef(40.0, 0.0, 0.0, 1.0);
				glTranslatef(0.3, 0.0, 0.0);
				drawLimb(RARM, NULL, NULL);
				glPopMatrix();
			// Move position for R elbow
			glPushMatrix();
			glTranslatef(0.8, 0.12, 0.0);
			drawJoint(&rElbow);
			glRotatef((GLfloat) rElbow.rotX(), 1.0, 0.0, 0.0);
			glRotatef((GLfloat) rElbow.rotY(), 0.0, 1.0, 0.0);
			glRotatef((GLfloat) rElbow.rotZ(), 0.0, 0.0, 1.0);
			//Set position for Right forearm
			glTranslatef(0.6, 0.12, 0.0);
			glPushMatrix();
			glRotatef(30.0, 0.0, 1.0, 0.0);
			glRotatef(-220.0, 0.0, 0.0, 1.0);
			glTranslatef(0.0, 0.2, 0.0);
			drawLimb(RFOREARM, NULL, NULL);
			glPopMatrix();
			//Move position for R wrist
			glTranslatef(0.6, -0.12, 0.0);
			drawJoint(&rWrist);
			glRotatef((GLfloat) rWrist.rotX(), 1.0, 0.0, 0.0);
			glRotatef((GLfloat) rWrist.rotY(), 0.0, 1.0, 0.0);
			glRotatef((GLfloat) rWrist.rotZ(), 0.0, 0.0, 1.0);
			glTranslatef(0.6, -0.12, 0.0);
			glPushMatrix();
			glScalef(0.5, 0.5, 0.5);
			glTranslatef(-0.4, 0.0, 0.0);
			glRotatef(-20.0, 0.0, 0.0, 1.0);
			drawLimb(RHAND, NULL, NULL);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	//Move position for L shoulder bone
	glTranslatef(-0.5, 0.3, 0.0);
	drawJoint(&lShoulder);
	glRotatef((GLfloat) lShoulder.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) lShoulder.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) lShoulder.rotZ(), 0.0, 0.0, 1.0);
	// Set position for Left Arm
	glTranslatef(-0.4, 0.2, 0.0);
	glPushMatrix();
	glScalef(1.0, 1.0, 1.0);
	glRotatef(-140.0, 0.0, 0.0, 1.0);
	glTranslatef(0.3, 0.1, 0.0);
	drawLimb(LARM, NULL, NULL);
	glPopMatrix();
	// Move position for L elbow
	glPushMatrix();
	glTranslatef(-0.8, -0.25, 0.0);
	drawJoint(&lElbow);
	glRotatef((GLfloat) lElbow.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) lElbow.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) lElbow.rotZ(), 0.0, 0.0, 1.0);
	//Set position for Left forearm
	glTranslatef(-0.6, -0.15, 0.0);
	glPushMatrix();
	glRotatef(-30.0, 0.0, 1.0, 0.0);
	glRotatef(-30.0, 0.0, 0.0, 1.0);
	glTranslatef(0.0, 0.0, 0.0);
	drawLimb(LFOREARM, NULL, NULL);
	glPopMatrix();
	//Move position for L wrist
	glTranslatef(-0.7, -0.12, 0.0);
	drawJoint(&lWrist);
	glRotatef((GLfloat) lWrist.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) lWrist.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) lWrist.rotZ(), 0.0, 0.0, 1.0);
	glTranslatef(-0.7, -0.12, 0.0);
	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);
	glTranslatef(0.4, 0.0, 0.0);
	glRotatef(-20.0, 0.0, 0.0, 1.0);
	drawLimb(LHAND, NULL, NULL);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();
	// Set Waist
	glTranslatef(0.0, -1.5, 0.0);
	glPushMatrix();
	drawLimb(HIP, NULL, NULL);
	glPopMatrix();

	glPushMatrix();
	//Set Coordinates for Right Leg
	glTranslatef(0.25, -0.5, 0.0);
	drawJoint(&rLeg);
	glRotatef((GLfloat) rLeg.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) rLeg.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) rLeg.rotZ(), 0.0, 0.0, 1.0);
	// Set position for Right Thigh
	glTranslatef(0.05, -0.7, 0.15);
	glPushMatrix();
	glRotatef(10, 0.0, 0.0, 1.0);
	glRotatef(-10, 1.0, 0.0, 0.0);
	glScalef(1.3, 1.5, 1.3);
	drawLimb(RTHIGH, NULL, NULL);
	glPopMatrix();
	// Move position for R knee
	glPushMatrix();
	glTranslatef(0.0, -0.7, 0.1);
	drawJoint(&rKnee);
	glRotatef((GLfloat) rKnee.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) rKnee.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) rKnee.rotZ(), 0.0, 0.0, 1.0);
	//Set position for Right Calf
	glTranslatef(0.0, -0.7, 0.0);
	glPushMatrix();
	glRotatef(10, 0.0, 0.0, 1.0);
	glRotatef(-20, 1.0, 0.0, 0.0);
	glScalef(1.3, 1.5, 1.3);
	drawLimb(RCALF, NULL, NULL);
	glPopMatrix();
	//Move position for R Ankle
	glTranslatef(0.0, -0.8, 0.0);
	drawJoint(&rAnkle);
	glRotatef((GLfloat) rAnkle.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) rAnkle.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) rAnkle.rotZ(), 0.0, 0.0, 1.0);
	glTranslatef(0.0, -0.3, 0.2);
	// Set position for Right Foot
	glPushMatrix();
	drawLimb(RFOOT, NULL, NULL);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	//Set Coordinates for Left Leg
	glTranslatef(-0.25, -0.5, 0.0);
	drawJoint(&lLeg);
	glRotatef((GLfloat) lLeg.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) lLeg.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) lLeg.rotZ(), 0.0, 0.0, 1.0);
	// Set position for Left Thigh
	glTranslatef(-0.05, -0.7, 0.15);
	glPushMatrix();
	glRotatef(10, 0.0, 0.0, 1.0);
	glRotatef(-10, 1.0, 0.0, 0.0);
	glScalef(1.3, 1.5, 1.3);
	drawLimb(LTHIGH, NULL, NULL);
	glPopMatrix();
	// Move position for L knee
	glPushMatrix();
	glTranslatef(0.0, -0.7, 0.0);
	drawJoint(&lKnee);
	glRotatef((GLfloat) lKnee.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) lKnee.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) lKnee.rotZ(), 0.0, 0.0, 1.0);
	//Set position for Left Calf
	glTranslatef(0.0, -0.7, 0.0);
	glPushMatrix();
	glRotatef(10, 0.0, 0.0, 1.0);
	glRotatef(-10, 1.0, 0.0, 0.0);
	glScalef(1.3, 1.5, 1.3);
	drawLimb(LCALF, NULL, NULL);
	glPopMatrix();
	//Move position for L Ankle
	glTranslatef(0.0, -0.8, 0.0);
	drawJoint(&lAnkle);
	glRotatef((GLfloat) lAnkle.rotX(), 1.0, 0.0, 0.0);
	glRotatef((GLfloat) lAnkle.rotY(), 0.0, 1.0, 0.0);
	glRotatef((GLfloat) lAnkle.rotZ(), 0.0, 0.0, 1.0);
	glTranslatef(0.0, -0.3, 0.2);
	// Set position for Left Footr
	glPushMatrix();
	glRotatef(-10.0, 0.0, 1.0, 0.0);
	drawLimb(LFOOT, NULL, NULL);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
	// Swap The Buffers To Not Be Left With A Clear Screen
}

void reshape(int width, int height) // Create The Reshape Function (the viewport)
{
	float aspect;

	  winsize.x = width;
	  winsize.y = height;
	  winorig.x = 0;
	  winorig.y = 0;

	  aspect = (float)width / (float)height;

	  glViewport(0, 0, winsize.x, winsize.y); // Reset The Current Viewport

	  	glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
	  	glLoadIdentity(); // Reset The Projection Matrix

	  	// Calculate The Aspect Ratio Of The Window
	  	gluPerspective(60.0f, aspect, 0.1f, 100.0f);

	  	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
	  	glLoadIdentity();
}

void mouseClick(int button, int state, int x, int y) {
	if(mouseFocus==FIGURE) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		mousedw(x, y, button);
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		mouseNow.x = x;
		  mouseNow.y = winsize.y - y;
		  vNow.x = 2.0 * ((float)mouseNow.x/(float)winsize.x) - 1.0;
		  vNow.y = 2.0 * ((float)mouseNow.y/(float)winsize.y) - 1.0;

		  act->Ball_Mouse(vNow);
		  if (state == GLUT_DOWN)
		    act->Ball_BeginDrag();
		  else
		    act->Ball_EndDrag();
	}
	}
	else if(mouseFocus==LIGHT) {
		newLightPost(winsize.x,winsize.y-y,5);
	}
	glutPostRedisplay();
}

void mouseMotion (int x, int y)
{
	 mouseNow.x = x;
	  mouseNow.y = winsize.y - y;
	  vNow.x = 2.0 * ((float)mouseNow.x/(float)winsize.x) - 1.0;
	  vNow.y = 2.0 * ((float)mouseNow.y/(float)winsize.y) - 1.0;
	  act->Ball_Mouse(vNow);
	  act->Ball_Update();
  glutPostRedisplay();
}

void mousedw(int x, int y, int but) {
	printf("Mouse button %d pressed at %d %d\n", but, x, y);
	gl_select(x, winsize.y - y); //Important: gl (0,0) ist bottom left but window coords (0,0) are top left so we have to change this!
	oldX = x; oldY = y;
}

void gl_select(int x, int y) {
	GLuint buff[64] = { 0 };
	GLint hits, view[4];
	printf("%s", "clicked");

	/*
	 This choose the buffer where store the values for the selection data
	 */
	glSelectBuffer(64, buff);

	/*
	 This retrieve info about the viewport
	 */
	glGetIntegerv(GL_VIEWPORT, view);

	/*
	 Switching in selecton mode
	 */
	glRenderMode(GL_SELECT);

	/*
	 Clearing the name's stack
	 This stack contains all the info about the objects
	 */
	glInitNames();

	/*
	 Now fill the stack with one element (or glLoadName will generate an error)
	 */
	glPushName(0);

	/*
	 Now modify the vieving volume, restricting selection area around the cursor
	 */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	/*
	 restrict the draw to an area around the cursor
	 */
	gluPickMatrix(x, y, 1.0, 1.0, view);
	gluPerspective(60, 1.0, 0.0001, 1000.0);

	/*
	 Draw the objects onto the screen
	 */
	glMatrixMode(GL_MODELVIEW);

	/*
	 draw only the names in the stack, and fill the array
	 */
	glutSwapBuffers();
	display();

	/*
	 Do you remeber? We do pushMatrix in PROJECTION mode
	 */
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	/*
	 get number of objects drawed in that area
	 and return to render mode
	 */
	hits = glRenderMode(GL_RENDER);

	/*
	 Print a list of the objects
	 */
	list_hits(hits, buff);
	/*
	 uncomment this to show the whole buffer
	 * /
	 gl_selall(hits, buff);
	 */

	glMatrixMode(GL_MODELVIEW);

}

void gl_selall(GLint hits, GLuint *buff) {
	GLuint *p;
	int i;

	display();

	p = buff;
	for (i = 0; i < 6 * 4; i++) {
		printf("Slot %d: - Value: %d\n", i, p[i]);
	}

	printf("Buff size: %x\n", (GLbyte) buff[0]);
}

void list_hits(GLint hits, GLuint *names) {
	int i;

	/*
	 For each hit in the buffer are allocated 4 bytes:
	 1. Number of hits selected (always one,
	 beacuse when we draw each object
	 we use glLoadName, so we replace the
	 prevous name in the stack)
	 2. Min Z
	 3. Max Z
	 4. Name of the hit (glLoadName)
	 */

	printf("%d hits:\n", hits);

	for (i = 0; i < hits; i++) {
	/*	printf("Number: %d\n"
			"Min Z: %d\n"
			"Max Z: %d\n"
			"Name on stack: %d\n", (GLubyte) names[i * 4], (GLubyte) names[i
				* 4 + 1], (GLubyte) names[i * 4 + 2],
				(GLubyte) names[i * 4 + 3]); */
		active = names[i * 4 + 3];
		limbJoint();
		glutPostRedisplay();
	}

	printf("\n");

}

void keyboard(unsigned char key, int x, int y) // Create Keyboard Function
{
	switch (key) {
	case 'n':
		act = &neck;
		break;
	case 'h':
		act = &pelvis;
		break;
	case 'u':
		act = &rShoulder;
		break;
	case 'i':
		act = &rElbow;
		break;
	case 'o':
		act = &rWrist;
		break;
	case 'j':
		act = &rLeg;
		break;
	case 'k':
		act = &rKnee;
		break;
	case 'l':
		act = &rAnkle;
		break;
	case 'r':
		act = &lShoulder;
		break;
	case 'e':
		act = &lElbow;
		break;
	case 'w':
		act = &lWrist;
		break;
	case 'f':
		act = &lLeg;
		break;
	case 'd':
		act = &lKnee;
		break;
	case 's':
		act = &lAnkle;
	case 'x':
		rMode = 1;
		break;
	case 'c':
		nullConstraints();
		break;
	case 'y':
		rMode = 2;
		break;
	case 'z':
		rMode = 0;
		break;
	case ' ':
		act = &body;
		break;
	case 27: // When Escape Is Pressed...
		exit(0); // Exit The Program
		break; // Ready For Next Case
	case '+':
 cam += 0.2;
		break;
	case '-':
 cam -= 0.2;
		break;
	case 'b':
		changeLightPower(0.1);
		break;
	case 'v':
		changeLightPower(-0.1);
		break;
	case 'L':
		mouseFocus =LIGHT;
		break;
	case 'F':
		mouseFocus = FIGURE;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void arrow_keys(int a_keys, int x, int y) // Create Special Function (required for arrow keys)
{
	switch (a_keys) {
	case GLUT_KEY_UP: // When Up Arrow Is Pressed..
		moveY += 0.3;
		break;
	case GLUT_KEY_DOWN: // When Down Arrow Is Pressed...
		moveY -= 0.3;
		break;
	case GLUT_KEY_LEFT:
		switch (rMode) {
		case 1:
			act->rotateX(-1);
			break;
		case 2:
			act->rotateY(-1);
			break;
		case 0:
			act->rotateZ(-1);
			break;
		default:
			break;
		}
		break;
		break;
	case GLUT_KEY_RIGHT:
		switch (rMode) {
		case 1:
			act->rotateX(1);
			break;
		case 2:
			act->rotateY(1);
			break;
		case 0:
			act->rotateZ(1);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) // Create Main Function For Bringing It All Together
{
	glutInit(&argc, argv); // Erm Just Write It =)
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // Display Mode
	winsize.x = 500;
	winsize.y = 500;
	glutInitWindowSize(winsize.x, winsize.y); // If glutFullScreen wasn't called this is the window size
	glutCreateWindow("Project Dolly"); // Window Title (argv[0] for current directory as title)
	init();
	glutDisplayFunc(display); // Matching Earlier Functions To Their Counterparts
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(arrow_keys);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	// glutIdleFunc		  ( display );
	glutMainLoop(); // Initialize The Main Loop
	return 0;
}

