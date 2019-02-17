/*******************************************************************
           Multi-Part Model Construction and Manipulation
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "QuadMesh.h"
#include <windows.h>

const int meshSize = 16;    // Default Mesh Size
const int vWidth = 650;     // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

static int currentButton;
static unsigned char currentKey;

// Lighting/shading and material properties for drone - upcoming lecture - just copy for now

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat drone_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat drone_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat drone_mat_diffuse[] = { 0.9F, 1.0F, 0.0F, 1.0F };
static GLfloat drone_mat_shininess[] = { 0.0F };

// A quad mesh representing the ground
static QuadMesh groundMesh;

// Structure defining a bounding box, currently unused
//struct BoundingBox {
//    Vector3D min;
//    Vector3D max;
//} BBox;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
Vector3D ScreenToWorld(int x, int y);

static GLfloat xr = 0, yr = 0, zr = 0; //initialize variables for translation


int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(vWidth, vHeight);
    glutInitWindowPosition(200, 30);
    glutCreateWindow("Assignment 1");

    // Initialize GL
    initOpenGL(vWidth, vHeight);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotionHandler);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(functionKeys);

    // Start event loop, never returns
    glutMainLoop();
    return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). */
void initOpenGL(int w, int h)
{
    // Set up and enable lighting
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);   // This light is currently off

    // Other OpenGL setup
    glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
    glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
    glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

    // Set up ground quad mesh
    Vector3D origin = NewVector3D(-8.0f, 0.0f, 8.0f);
    Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
    Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
    groundMesh = NewQuadMesh(meshSize);
    InitMeshQM(&groundMesh, meshSize, origin, 16.0, 16.0, dir1v, dir2v);

    Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
    Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
    Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
    SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

    // Set up the bounding box of the scene
    // Currently unused. You could set up bounding boxes for your objects eventually.
    //Set(&BBox.min, -8.0f, 0.0, -8.0);
    //Set(&BBox.max, 8.0f, 6.0,  8.0);
}



void draw1() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0.0f + xr, 2.0f + yr, 0.0f + zr);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(1.0f + xr, 2.0f + yr, 0.0f + zr);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(1.0f + xr, 2.0f + yr, -5.0f + zr);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(0.0f + xr, 2.0f + yr, -5.0f + zr);	//top face
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-2.0f + xr, 2.0f + yr, -2.0f + zr);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(3.0f + xr, 2.0f + yr, -2.0f + zr);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(3.0f + xr, 2.0f + yr, -3.0f + zr);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(-2.0f + xr, 2.0f + yr, -3.0f + zr);

	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}

void draw2() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(0.0f + xr, 1.0f + yr, 0.0f + zr);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(1.0f + xr, 1.0f + yr, 0.0f + zr);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(1.0f + xr, 1.0f + yr, -5.0f + zr);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(0.0f + xr, 1.0f + yr, -5.0f + zr);	//botton face
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-2.0f + xr, 1.0f + yr, -2.0f + zr);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(3.0f + xr, 1.0f + yr, -2.0f + zr);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(3.0f + xr, 1.0f + yr, -3.0f + zr);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-2.0f + xr, 1.0f + yr, -3.0f + zr);

	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}


void draw3() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(0.0f + xr, 2.0f + yr, 0.0f + zr);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(0.0f + xr, 1.0f + yr, 0.0f + zr);	//middle front face 3
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(1.0f + xr, 1.0f + yr, 0.0f + zr);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(1.0f + xr, 2.0f + yr, 0.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw4() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(1.0f + xr, 2.0f + yr, -2.0f + zr);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(1.0f + xr, 1.0f + yr, -2.0f + zr);	//right front face 4
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(3.0f + xr, 1.0f + yr, -2.0f + zr);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(3.0f + xr, 2.0f + yr, -2.0f + zr);

	
	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}

void draw5() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(0.0f + xr, 2.0f + yr, -2.0f + zr);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-2.0f + xr, 2.0f + yr, -2.0f + zr);	//left front face 5
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(-2.0f + xr, 1.0f + yr, -2.0f + zr);
	glNormal3f(0.0, 0.0, 1.0);
	glVertex3f(0.0f + xr, 1.0f + yr, -1.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw6() {
	glPushMatrix();
	glBegin(GL_QUADS);

	
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(0.0f + xr, 2.0f + yr, -5.0f + zr);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(0.0f + xr, 1.0f + yr, -5.0f + zr);	//middle back face 6
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(1.0f + xr, 1.0f + yr, -5.0f + zr);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(1.0f + xr, 2.0f + yr, -5.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw7() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(1.0f + xr, 2.0f + yr, -3.0f + zr);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(1.0f + xr, 1.0f + yr, -3.0f + zr);	//right back face 7
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(3.0f + xr, 1.0f + yr, -3.0f + zr);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(3.0f + xr, 2.0f + yr, -3.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw8() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(0.0f + xr, 2.0f + yr, -3.0f + zr);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(-2.0f + xr, 2.0f + yr, -3.0f + zr);	//left back face 8
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(-2.0f + xr, 1.0f + yr, -3.0f + zr);
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(0.0f + xr, 1.0f + yr, -3.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw9() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-2.0f + xr, 2.0f + yr, -2.0f + zr);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-2.0f + xr, 2.0f + yr, -3.0f + zr);	//left outer face 9
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-2.0f + xr, 1.0f + yr, -3.0f + zr);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-2.0f + xr, 1.0f + yr, -2.0f + zr);



	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw10() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0f + xr, 2.0f + yr, 0.0f + zr);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0f + xr, 2.0f + yr, -2.0f + zr);	//left inner front face 10
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0f + xr, 1.0f + yr, -2.0f + zr);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0f + xr, 1.0f + yr, 0.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw11() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0f + xr, 2.0f + yr, -3.0f + zr);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0f + xr, 2.0f + yr, -5.0f + zr);	//left inner back face 11
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0f + xr, 1.0f + yr, -5.0f + zr);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(0.0f + xr, 1.0f + yr, -3.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw12() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(3.0f + xr, 2.0f + yr, -2.0f + zr);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(3.0f + xr, 2.0f + yr, -3.0f + zr);	//right outer face 12
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(3.0f + xr, 1.0f + yr, -3.0f + zr);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(3.0f + xr, 1.0f + yr, -2.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw13() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(1.0f + xr, 2.0f + yr, 0.0f + zr);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(1.0f + xr, 2.0f + yr, -2.0f + zr);	//right inner front face 13
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(1.0f + xr, 1.0f + yr, -2.0f + zr);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(1.0f + xr, 1.0f + yr, 0.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}
void draw14() {
	glPushMatrix();
	glBegin(GL_QUADS);

	glColor3f(1.0, 0, 1.0);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(1.0f + xr, 2.0f + yr, -3.0f + zr);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(1.0f + xr, 2.0f + yr, -5.0f + zr);	//right inner back face 14
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(1.0f + xr, 1.0f + yr, -5.0f + zr);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(1.0f + xr, 1.0f + yr, -3.0f + zr);


	glEnd();
	glPopMatrix();

	glutPostRedisplay();

}

void draw15() {
	glPushMatrix();

	glutSolidCube(1.0);			//draw propeller

	glPopMatrix();
	glutPostRedisplay();
}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.


//spin function
static GLfloat spin = 0.0;
void spinDisplay(void) {
	spin = spin + 10.0f;
	if (spin > 360.0)
		spin = spin - 360.0f;
	glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
	if (key == 'w') { if (zr >= -6.0) { zr -= 0.5; } } if (key == 's') { if (zr <= 12.0) { zr += 0.5; } }	//move back and forth
	if (key == 'a') { if (xr >= -6.0) { xr -= 0.5; }} if (key == 'd') { 1.0;  if (xr <= 6.0) { xr += 0.5; } }	//move left and right
	if (key == 'q') { if (yr <= 10.0) { yr += 0.5; } } if (key == 'z') {yr -= 0.5;}	//move up and down
	if (key == 'h') { glutIdleFunc(spinDisplay); }
	
}




void draw(){ //draws drone
	glColor3f(1.0f, 0.0f, 0.0f);
	draw1();
	draw2();
	draw3();
	draw4();
	draw5();
	draw6();
	draw7();
	draw8();
	draw9();
	draw10();
	draw11();
	draw12();
	draw13();
	draw14();
}




static GLfloat loop = -7.0f;

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*if (loop = 7.0f)
	{
		for (int i = 0; i < 14; i++)
		{
			loop -= 1.0f;
		}
	}

	if (loop = -7.0f)
	{
		for (int i = 0; i < 14; i++)
		{
			loop += 1.0f;
		}
	} */

	double move_direction = 1.0;
	 //it will oscillate back and forth forever
	while (loop < 5.0)
	{
		loop += 0.001f;

		if (loop == 5.0)
		{
			loop = -7.0f;
			
		}
	}
			
	

	//crash detection
	while (yr == -1.5f || xr == 5.5f || xr == -5.5f )
	{
		xr = 0.0f;
		yr = 0.0f;
		zr = 0.0f;
		printf("You Crashed :( \n");
		
	}
	


    // Set drone material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);

    // Apply transformations to move drone

	//road x axis
	glPushMatrix();
	glScalef(15.0f, 0.25f, 5.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	//road z axis
	glPushMatrix();
	glScalef(5.0f, 0.25f, 15.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	//left side buildings

	glPushMatrix();
	glTranslatef(-7.5f, 5.0f, -8.0f);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-7.5f, 5.0f, -6.0f);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-7.5f, 5.0f, -4.0f);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-7.5f, 5.0f, -2.0);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-7.5f, 5.0f, 0.0);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-7.5f, 5.0f, 2.0);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-7.5f, 5.0f, 4.0f);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-7.5f, 5.0f, 6.0f);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();


//right side buildings

	glPushMatrix();
	glTranslatef(7.5f, 5.0f, -8.0);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.5f, 5.0f, -6.0);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.5f, 5.0f, -4.0);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(7.5f, 5.0f, -2.0);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(7.5f, 5.0f, 0.0);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(7.5f, 5.0f, 2.0);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();
	

	glPushMatrix();
	glTranslatef(7.5f, 5.0f, 4.0f);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(7.5f, 5.0f, 6.0f);
	glScalef(1.0f, 10.0f, 1.0f);
	glutSolidCube(1.0);
	glPopMatrix();


	//draw drone
	glPushMatrix();
	glTranslatef(-0.5, 0.0, -2.5);
	draw();
	glPopMatrix();

	//second drone
	glPushMatrix();
	glTranslatef(-1.0f, 4.0f, 8.0f);
	glScalef(3.0f, 1.0f, 3.0f);
	glutSolidCube(1.0);
	glPopMatrix(); 

	//front propellor stick
	glPushMatrix();
	glTranslatef(-1.0f, 5.0f, 8.0f);
	glRotatef(25.0f + spin, 0.0f, 1.0f, 0.0f);
	glScalef(0.1f, 2.5f, 0.1f);
	glutSolidCube(1.0);
	glPopMatrix();

	//front propellor blade
	glPushMatrix();
	glTranslatef(-1.0f, 6.245f, 8.0f);
	glRotatef(25.0f + spin, 0.0f, 1.0f, 0.0f);
	glScalef(0.5f, 0.1f, 2.5f);
	draw15();
	glPopMatrix();

	//drone #2 propellor stick
	glPushMatrix();
	glTranslatef(0.0f + xr, 2.255f + yr, -7.0f+ zr);
	glRotatef(45.0f + spin, 0.0f, 1.0f, 0.0f);
	glScalef(0.1f, 2.5f, 0.1f);							
	glutSolidCube(1.0);
	glPopMatrix();
	
	//propellor blade
	glPushMatrix();
	glTranslatef(0.0f + xr, 3.5f + yr, -7.0f + zr);
	glRotatef(45.0f + spin , 0.0f, 1.0f, 0.0f);
	glScalef(0.5f, 0.1f, 2.5f);
	draw15();
	glPopMatrix();


	//front propellor stick
	glPushMatrix();
	glTranslatef(0.0f + xr, 2.255f + yr, -3.0f+ zr);
	glRotatef(65.0f + spin, 0.0f, 1.0f, 0.0f);
	glScalef(0.1f, 2.5f, 0.1f);
	glutSolidCube(1.0);
	glPopMatrix();

	//front propellor blade
	glPushMatrix();
	glTranslatef(0.0f + xr, 3.5f + yr, -3.0f + zr);
	glRotatef(65.0f + spin, 0.0f, 1.0f, 0.0f);
	glScalef(0.5f, 0.1f, 2.5f);
	draw15();
	glPopMatrix();

	//right propellor stick
	glPushMatrix();
	glTranslatef(2.0f + xr, 2.255f+ yr, -5.0f + zr);
	glRotatef(25.0f + spin, 0.0f, 1.0f, 0.0f);
	glScalef(0.1f, 2.5f, 0.1f);
	glutSolidCube(1.0);
	glPopMatrix();

	//right propellor blade
	glPushMatrix();
	glTranslatef(2.0f + xr, 3.5f + yr, -5.0f + zr);
	glRotatef(25.0f + spin, 0.0f, 1.0f, 0.0f);
	glScalef(0.5f, 0.1f, 2.5f);
	draw15();
	glPopMatrix();



	//left propellor stick
	glPushMatrix();
	glTranslatef(-2.0f + xr, 2.255f + yr, -5.0f + zr);
	glRotatef(15.0f + spin, 0.0f, 1.0f, 0.0f);
	glScalef(0.1f, 2.5f, 0.1f);
	glutSolidCube(1.0);
	glPopMatrix();

	//left propellor blade
	glPushMatrix();
	glTranslatef(-2.0f+ xr, 3.5f + yr, -5.0f + zr);
	glRotatef(15.0f + spin, 0.0f, 1.0f, 0.0f);
	glScalef(.5f, 0.1f, 2.5f);
	draw15();
	glPopMatrix();

    // Apply transformations to construct drone, modify this!

	
	glutPostRedisplay();
    // Draw ground mesh
    DrawMeshQM(&groundMesh, meshSize);

    glutSwapBuffers();   // Double buffering, swap buffers
}


// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
    // Set up viewport, projection, then change to modelview matrix mode - 
    // display function will then set up camera and do modeling transforms.
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
    gluLookAt(10.0, 7.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Callback, handles input from the keyboard, non-arrow keys
/*void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 't':
	glRotatef(spin,1.0,0.0,0.0);
        break;
	case 'y':
		glRotatef(spin, 0.0, 1.0, 0.0);
		break;
	case 'u':
		glRotatef(spin, 0.0, 0.0, 1.0);
		break;
    }

    glutPostRedisplay();   // Trigger a window redisplay
}*/

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
    // Help key
    if (key == GLUT_KEY_F1)
    {
		MessageBoxA(NULL, "Use 'w a s d' keys to move around, 'q' and 'z' to move up and down. Press 'h' to start rotaters", "Controls", MB_OK | MB_ICONEXCLAMATION); //help button
    }
    // Do transformations with arrow keys
    //else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
    //{
    //}

    glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
    currentButton = button;

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        {
			glutIdleFunc(spinDisplay);

        }
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN)
        {
           glutIdleFunc(NULL);
        }
        break;
    default:
        break;
    }

    glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
    if (currentButton == GLUT_LEFT_BUTTON)
    {
        ;
    }

    glutPostRedisplay();   // Trigger a window redisplay
}


Vector3D ScreenToWorld(int x, int y)
{
    // you will need to finish this if you use the mouse
    return NewVector3D(0, 0, 0);
}




