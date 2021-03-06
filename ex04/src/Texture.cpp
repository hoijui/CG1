/* ----------------------------------------------------------------
name:           Texture.cpp
purpose:        texturing tutorial
'introduction to computer graphics'
winter term 2012/2013, assignment 4
version:	   SKELETON CODE
TODO:           see XXX
author:         katrin lang
computer graphics
tu berlin
------------------------------------------------------------- */

#ifdef __APPLE__
#include <GLUT/freeglut.h>
#elif _WIN32
#include "win32/freeglut.h"
#else
//#include <GL/freeglut.h>
#endif

#include <string>
#include <iostream>

//#include "glm/glm.hpp"

#include "GLSLShader.h"
#include "Context.hpp"
#include "Texture.hpp"
#include "Image.hpp"
#include "Mesh.h"

#define DEBUG_SHADER 0

using namespace glm;
using namespace std;

static const float PI= 3.14159265358979323846264338327950288;

// current state of mouse action
static enum {
	ROTATE, SHIFT_XY, SHIFT_Z, SCALE, NO_DRAG, DRAW, ERASE
} drag= NO_DRAG;

// screen size
static vec2 screen;

static bool showTexture= true;
static bool textureCorrection= true;
static bool doLighting= true;
static bool showCoordinates= true;
static bool showOrigin= true;
#if DEBUG_SHADER
#define EM true
#define DR false
#else
#define EM false
#define DR true
#endif
static bool environmentMapping= EM;
static bool drawRect= DR;

static GLuint modulation= GL_MODULATE;

static vec2 rotation= vec2(0); // current rotation of object
static vec3 shift= vec3(0); // offset
static float scale= 1; // scale
static vec3 cursor= vec3(1,0,0);
static float fov= 60.0; // field of view
// camera setup
// camera position
static GLfloat cameraZ;
// near and far plane
static GLfloat nearPlane;
static GLfloat farPlane;

static Image texture;
#if DEBUG_SHADER
#define DTI 10
#define DMI 13
#else
#define DTI 1
#define DMI -1
#endif
static int defaultTextureIndex = DTI;
static int defaultModelIndex = DMI;
static int  isSet = 0;
static int shaderIsLoaded = 0;
GLSLShader myShader;

/*************************************************************************************/

// calculate cursor position
// XXX: NEEDS TO BE IMPLEMENTED
static void updateCursor(int x, int y) {

	// XXX

	// INSERT YOUR CODE HERE
	float u = (float) x / screen.x;
	float v = (float) y / screen.y;
	float u_rad = 2 * PI * u;
	cursor.x = sin(u_rad - PI / 2);;
	cursor.z = sin(u_rad);
	// y goes from +1 (north) to -1 (south)
	// v from 0 to 1, hence:
	// *2 to expand range, -1 to shift range & the whole *(-1) to flip it
	cursor.y = - (v * 2 - 1);

	// END XXX
}

static void reset() {

	rotation = vec2(0); // current rotation of object
	shift = vec3(0); // offset
	fov = 60.0;
}

void Common::keyPressed(unsigned char key, int x, int y) {

	switch (key) {
		case 'q':
		case 'Q':
			exit(EXIT_SUCCESS);
			break;

		case 'r':
			reset();
			break;

		case 's':
			scale *= 0.9;
			break;

		case 'S':
			scale *= 1.1;
			break;

		default:
			break;
	}

	Context::display();
}

// draw a full screen quad
// XXX: NEEDS TO BE IMPLEMENTED
static void fullScreenQuad() {
	// XXX

	// INSERT YOUR CODE HERE
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, 0.0);
	glEnd();

	// END XXX
}

// -------------------------------------------------------
// TEXTURE WINDOW
// -------------------------------------------------------

int Texture::menuOptions[]=  {0, 17, 18, 0, 1, 2, 3, 4, 5, 0, 6, 7, 8, 9, 10, 11, 12, 0, 13, 14, 15, 16, 0, 19, 20, 21, 22, 23, 24, 25, 26};
string Texture::menuText[]= {"TOOLS:", "    Pen", "    Eraser",
	"SPHERICAL TEXTURES:", "    Earth", "    Earth (ice)", "    Earth (night)", "    Saturn", "    Marble",
	"ENVIRONMENT TEXTURES:", "    St Peters dome", "    Uffizium", "    Supernova", "    Landscape", "    Forest", "    Spectrum", "    Phong Light",
	"MISC TEXTURES", "    Checkerboard", "    Grid1", "    Grid2", "    Nemo",
	"FILTERING", "    mag: NEAREST",  "    mag: LINEAR", "    min: NEAREST" , "    min: LINEAR", "    min: NEAREST_MIPMAP_NEAREST  ", "    min: LINEAR_MIPMAP_NEAREST", "    min: NEAREST_MIPMAP_LINEAR", "    min: LINEAR_MIPMAP_LINEAR"};
int Texture::numOptions= 31;

string textures[]= {"", "data/earthcyl2.ppm", "data/earth2.ppm", "data/earthlights.ppm", "data/saturncyl1.ppm", "data/marble.ppm", "data/stpeters.ppm", "data/uffizi.ppm", "data/supernova.ppm", "data/test5b.ppm", "data/test7b.ppm", "data/test2b.ppm", "data/test6b.ppm", "data/checker2.ppm", "data/test3b.ppm", "data/test4b.ppm", "data/test8b.ppm"};

vec2 Texture::previousMouse; // previous mouse position

void Texture::reshape(int width, int height) {

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, width, height);

	gluOrtho2D(0, width, 0, height);

	screen = vec2(width, height);
}



// display texture
// XXX: NEEDS TO BE IMPLEMENTED
void Texture::display() {

	// setup model matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// display texture
	// XXX

	// INSERT YOUR CODE HERE
	if (defaultTextureIndex != -1) {
		texture.load(textures[defaultTextureIndex]);
		texture.generateTexture();
		defaultTextureIndex = -1;
	}
	glEnable(GL_TEXTURE_2D);
	texture.setModulation(GL_DECAL);
	texture.bind();
	fullScreenQuad();
	glDisable(GL_TEXTURE_2D);
	// END XXX

	glutSwapBuffers();
}

void Texture::mousePressed(int button, int state, int x, int y) {

	if (button == GLUT_DOWN) {
		previousMouse = vec2(x, y);
	}
}

// mouse dragged callback
// XXX: NEEDS TO BE IMPLEMENTED
void Texture::mouseDragged(int x, int y) {

	// paint on texture
	// XXX

	// INSERT YOUR CODE HERE
	if (drag == DRAW) {
		texture.paint(x, y);
	} else if (drag == ERASE) {
		texture.erase(x, y);
	}

	// END XXX

	updateCursor(x, y);

	previousMouse= vec2(x, y);

	Context::display();
}

void Texture::mouseMoved(int x, int y) {

	if (x > 0 && x < screen.x && y > 0 && y < screen.y) {
		updateCursor(x, y);
		Context::display();
	}
}

// menu callback
// XXX: NEEDS TO BE IMPLEMENTED
void Texture::menu(int value) {

	switch(value) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
			texture.load(textures[value]);
			texture.generateTexture();
			if (value < 6)
				environmentMapping = false;
			else if (value < 13)
				environmentMapping = true;
			break;
		case 17:
			drag = DRAW;
			break;
		case 18:
			drag = ERASE;
			break;
//	"FILTERING", "    mag: NEAREST",  "    mag: LINEAR", "    min: NEAREST" , "    min: LINEAR", "    min: NEAREST_MIPMAP_NEAREST  ", "    min: LINEAR_MIPMAP_NEAREST", "//   min: NEAREST_MIPMAP_LINEAR", "    min: LINEAR_MIPMAP_LINEAR"};

		// add cases for texture filtering
		// XXX

		// INSERT YOUR CODE HERE
		case 19:
			texture.setMagFilter(GL_NEAREST);
			break;
		case 20:
			texture.setMagFilter(GL_LINEAR);
			break;
		case 21:
			texture.setMinFilter(GL_NEAREST);
			break;
		case 22:
			texture.setMinFilter(GL_LINEAR);
			break;
		case 23:
			texture.setMinFilter(GL_NEAREST_MIPMAP_NEAREST);
			break;
		case 24:
			texture.setMinFilter(GL_LINEAR_MIPMAP_NEAREST);
			break;
		case 25:
			texture.setMinFilter(GL_NEAREST_MIPMAP_LINEAR);
			break;
		case 26:
			texture.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
			break;

		// END XXX
		default:
			break;
	}

	Context::display();
}

// -------------------------------------------------------
// WORLD-SPACE WINDOW
// -------------------------------------------------------

int World::menuOptions[]= {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	0, 15, 16, 17, 18, 19, 20, 21};
string World::menuText[]= {"MODEL", "   Plane", "   Spiky Sphere", "   Car", "   Bunny", "   Cone", "   Cow", "   Cowboy Hat", "   Dragon", "   Chess", "   Temple", "   Cup", "   Space Shuttle", "   Sphere", "   None",
	"RENDERING", "   Lighting on/off", "   Texture on/off", "   Coordinate System on/off", "   Origin on/off",
	"   Texture Mode (DECAL/MODULATE) ", "   Texture Coordinate Correction on/off  ", "   Environment mapping on/off"};

static string models[]= {"", "", "data/4cow.off", "data/auto3.off", "data/bunny2.off", "data/cone.off", "data/cow.off", "data/cowboyhut.off", "data/MEGADRACHE.off", "data/Schachfigur.off", "data/tempel.off", "data/tasse.off", "data/spaceshuttle.off", "data/sphere.off"};

static Mesh model;

int World::numOptions= 23;

vec2 World::previousMouse;

void World::reshape(int width, int height) {

	// setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, width, height);

	cameraZ = 1 / tan(fov / 180.0);
	// near and far plane
	nearPlane = cameraZ / 10.0;
	farPlane = cameraZ * 10.0;

	gluPerspective(fov, width / height, nearPlane, farPlane);

	screen = vec2(width, height);
}

// display callback
// XXX: NEEDS TO BE IMPLEMENTED
void World::display() {

	glClearColor(0.2, 0.2, 0.2, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// position the camera at (0,0,cameraZ) looking down the
	// negative z-axis at (0,0,0)
	gluLookAt(0.0, 0.0, cameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	GLfloat lightPosition[]= { 5.0, 5.0, 5.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);

	// shift object
	glTranslatef(shift.x, shift.y, shift.z);

	// rotate Object
	glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotation.y, 0.0f, 1.0f, 0.0f);

	// show coordinate system
	if (showCoordinates) {
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINES);
		glColor3ub(255, 0, 0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);

		glColor3ub(0, 255, 0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);

		glColor3ub(0, 0, 255);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);
		glEnd();
	}

	// show center of spherical mapping
	if (showOrigin) {
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.0, 1.0, 0.0);
		glPushMatrix();

		glutSolidSphere(0.05f, 20, 20);
		glPopMatrix();
	}

	// draw cursor
	// XXX

	// INSERT YOUR CODE HERE
	glBegin(GL_LINES);
	glColor3ub(255, 0, 0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(cursor.x, cursor.y, cursor.z);
	glEnd();

	// END XXX

	glScalef(scale, scale, scale);

	if (doLighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	// draw the geometry

	// if showTexture is true, enable texturing in opengl
	// XXX

	// INSERT YOUR CODE HERE
	if (showTexture) {
		glEnable(GL_TEXTURE_2D);
	}

	// END XXX

	glColor3f(1, 1, 1);

	if (environmentMapping) {
		if (!shaderIsLoaded) {
			myShader.load("shaders/emBlinnPhong");
			shaderIsLoaded = 1;
		}
		myShader.bindShader();
	}

	if (drawRect) {
		// draw a textured quad
		// XXX

		// INSERT YOUR CODE HERE
		fullScreenQuad();

		// END XXX
	}

	// else draw model
	// XXX

	// INSERT YOUR CODE HERE
	else {
		if (defaultModelIndex != -1) {
			model = Mesh::loadOff(models[defaultModelIndex]);
			defaultModelIndex = -1;
		}
		model.Display();
	}
	if (environmentMapping) { // if we used em, unbind shader afterwards
		myShader.unbindShader();
	}
	// glDisable(GL_TEXTURE_2D); ???
	// END XXX

	glutSwapBuffers();
}

void World::mousePressed(int button, int state, int x, int y) {

	int modifier;

	switch (button) {
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN) {
				previousMouse= vec2(x, y);
				modifier = glutGetModifiers();
				if (modifier & GLUT_ACTIVE_CTRL)
					drag = SHIFT_XY;
				else if (modifier & GLUT_ACTIVE_SHIFT)
					drag = SHIFT_Z;
				else if (modifier & GLUT_ACTIVE_ALT)
					drag = SCALE;
				else
					drag = ROTATE;
			} else if (state == GLUT_UP) {
				drag = NO_DRAG;
			}
			break;
		default: break;
	}
}

void World::mouseDragged(int x, int y) {

	switch (drag) {
		case ROTATE:
			rotation.y += x - previousMouse.x;
			rotation.x += y - previousMouse.y;
			break;
		case SHIFT_XY:
			shift.x += (x - previousMouse.x) / 50.0;
			shift.y -= (y - previousMouse.y) / 50.0;
			break;
		case SHIFT_Z:
			shift.z += ((x - previousMouse.x) - (y - previousMouse.y)) / 50.0;
			break;
		case SCALE:
			scale += ((x - previousMouse.x) - (y - previousMouse.y)) / 50.0;
			break;
		default:
			break;
	}
	previousMouse = vec2(x, y);
	Context::display();
}


// menu callback
// XXX: NEEDS TO BE IMPLEMENTED
void World::menu(int value) {

	switch(value) {
		case 0:
		case 1:
			drawRect = true;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			// load model from textures[value]
			// XXX

			// INSERT YOUR CODE HERE
			model = Mesh::loadOff(models[value]);

			// END XXX
			drawRect = false;
			break;
		case 14:
			// no model should be displayed with this option selected
			// XXX

			// INSERT YOUR CODE HERE
			drawRect = true;

			// END XXX
			break;
		case 15:
			doLighting = !doLighting;
			break;
		case 16:
			showTexture = !showTexture;
			break;
		case 17:
			showCoordinates = !showCoordinates;
			break;
		case 18:
			showOrigin= !showOrigin;
			break;
		case 19:
			if (modulation == GL_MODULATE)
				modulation = GL_DECAL;
			else
				modulation = GL_MODULATE;
			texture.setModulation(modulation);
			break;
		case 20:
			textureCorrection = !textureCorrection;
			break;
		case 21:
			environmentMapping = !environmentMapping;
			break;
		default:
			break;
	}

	Context::display();
}
