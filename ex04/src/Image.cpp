/* ----------------------------------------------------------------
name:           Image.cpp
purpose:        texturing tutorial
'introduction to computer graphics'
winter term 2012/2013, assignment 4
version:	   SKELETON CODE
TODO:           texture and mipmap generation, texture filtering, modulation, texel get, painting in texture
author:         katrin lang
computer graphics
tu berlin
------------------------------------------------------------- */


#ifdef __APPLE__
#include <GLUT/freeglut.h>
#elif _WIN32
#include "win32/freeglut.h"
#else
#include <GL/freeglut.h>
#endif

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Image.hpp"
#include "Context.hpp"

using namespace std;
using namespace glm;

#define BRUSH_SIZE 1
static GLubyte brush[BRUSH_SIZE][BRUSH_SIZE][4];
static GLubyte brushColor[4] = {255, 0, 0, 0};
static int brushIsSet = 0;


Image::Image()
	: width(0)
	, height(0)
	, wrap(GL_CLAMP_TO_BORDER)
	, min(GL_LINEAR)
	, mag(GL_LINEAR)
	, modulate(GL_MODULATE)
	, textureID(0)
{
}

Image::Image(int width, int height)
	: width(width)
	, height(height)
	, data(width*height)
	, wrap(GL_CLAMP_TO_BORDER)
	, min(GL_LINEAR)
	, mag(GL_LINEAR)
	, modulate(GL_MODULATE)
	, textureID(0)
{
}

Image::Image(const std::string& filename)
	: wrap(GL_CLAMP_TO_BORDER)
	, min(GL_LINEAR)
	, mag(GL_LINEAR)
	, modulate(GL_MODULATE)
	, textureID(0)
{
	load(filename);
}

Image::~Image() {
}

// generate OpenGL texture
// XXX: NEEDS TO BE IMPLEMENTED
void Image::generateTexture() {

	if (textureID == 0) {
		// generate texture id
		// XXX

		// INSERT YOUR CODE HERE
		glGenTextures(1, &textureID);

		// END XXX
	}
	bind(); // ??? -> yes, to enable options for the first time

	// texture filtering and repeat
	// XXX

	// INSERT YOUR CODE HERE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// END XXX

	//enable automatic mipmap generation
	// XXX

	// INSERT YOUR CODE HERE
	// OpenGL 3.0+
	//glGenerateMipmap(GL_TEXTURE_2D);
	// OpenGL 1.4+
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	// END XXX

	// upload texture data
	// XXX

	// INSERT YOUR CODE HERE
	// FIXME doesn't work :/
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT_VEC4, &data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ttexture);

	// END XXX
}

void Image::setMinFilter(GLuint min) {
	this->min= min;

	// set texture parameter
	// XXX

	// INSERT YOUR CODE HERE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->min);

	// END XXX
}

// set magnifying filter
// XXX: NEEDS TO BE IMPLEMENTED
void Image::setMagFilter(GLuint mag) {

	this->mag= mag;

	// set texture parameter
	// XXX

	// INSERT YOUR CODE HERE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->mag);

	// END XXX
}

void Image::setModulation(GLuint modulation) {
	this->modulate = modulation;
}

// bind texture
// XXX: NEEDS TO BE IMPLEMENTED
void Image::bind() {
	// bind texture
	// XXX

	// INSERT YOUR CODE HERE
	glBindTexture(GL_TEXTURE_2D, this->textureID);

	// END XXX

	// set modulation
	// XXX

	// INSERT YOUR CODE HERE
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, this->modulate);

	// END XXX
}

// unbind texture
// XXX: NEEDS TO BE IMPLEMENTED
void Image::unbind() {
	// XXX

	// INSERT YOUR CODE HERE

	// END XXX
}

int getDataIndex(int x, int y,int width, int height) {
	return x + width * y;
}

// read a pixel from image
// XXX: NEEDS TO BE IMPLEMENTED
vec4 Image::get(int x, int y) {

	// XXX

	// INSERT YOUR CODE HERE
	int index = getDataIndex(x, y, width, height);
	return originalData[index];

	// END XXX
}

// draw in texture
// XXX: NEEDS TO BE IMPLEMENTED
void Image::paint(int x, int y) {
	// XXX

	// INSERT YOUR CODE HERE
	if (brushIsSet == 0) {
		int i, j, k;
		for (i = 0; i < BRUSH_SIZE; i++) {
			for (j = 0; j < BRUSH_SIZE; j++) {
				for (k = 0; k < 4; k++) {
					brush[i][j][k] = brushColor[k];
				}
			}
		}
		brushIsSet = 1;
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, BRUSH_SIZE, BRUSH_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, &brush);

	// END XXX
}

// erase drawing from texture
// XXX: NEEDS TO BE IMPLEMENTED
void Image::erase(int x, int y) {
	// XXX

	// INSERT YOUR CODE HERE
	// FIXME: doesn't work :/
	//vec4 pixel = get(x, y);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, BRUSH_SIZE, BRUSH_SIZE, GL_RGBA, GL_FLOAT_VEC4, &pixel);

	// *3 because we work in rgb
	int index = 3 * (x + width * y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &(ttexture[index]));

	// END XXX
}

void Image::load(const std::string& filename) {

	data.clear();

	if (filename.substr(filename.size()-4, 4) == ".ppm") {
		loadPPM(filename);
	} else {
		cerr << "file " << filename << " is not a PPM file" << endl;
		return;
	}

	ttexture = (GLubyte*) loadPPMToChar(filename.c_str(), &width, &height);
}

void Image::loadPPM(const std::string& filename) {

	ifstream file(filename.c_str(), ios::binary);

	if (!file.is_open()) {
		cerr << "opening file " << filename << " failed" << endl;
		return;
	}

	// grab first two chars of the file and make sure that it has the
	// correct magic cookie for a raw PPM file.
	string magic;
	getline(file, magic);
	if (magic.substr(0, 2) != "P6") {
		cerr << "File " << filename << " is not a raw PPM file" << endl;
		return;
	}

	// grab the three elements in the header (width, height, maxval).
	string dimensions;
	do {
		getline(file, dimensions);
	} while(dimensions[0] == '#');

	stringstream(dimensions) >> width >> height;

	string max;
	getline(file, max);
	int maxValue;
	stringstream(max) >> maxValue;
	// grab all the image data in one fell swoop.
	vector<char> raw(width*height*3);
	file.read(&raw[0], raw.capacity());
	file.close();

	data.resize(width*height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			data[y*width+x]= vec4((unsigned char)raw[(height - y-1) * width * 3 + 3*x], (unsigned char)raw[(height - y-1) * width * 3 + 3*x + 1], (unsigned char)raw[(height - y-1) * width * 3 + 3*x + 2], maxValue);
			data[y*width+x]/= maxValue;
			//cout << data[i].r << " " + data[i].g << " " + data[i].b << " " + data[i].a << endl;
		}
	}

	raw.clear();
	originalData = data;

	std::cout << "Image " << filename << " loaded. width=" << width << " height=" << height << endl;
}

unsigned char* Image::loadPPMToChar(const char* filename, int* width, int* height ) {

	FILE* fp;
	int i, w, h, d;
	unsigned char* image;
	char head[70];		// max line <= 70 in PPM (per spec).

	fp = fopen( filename, "rb" );
	if (!fp) {
		perror(filename);
		return NULL;
	}

	// Grab first two chars of the file and make sure that it has the
	// correct magic cookie for a raw PPM file.
	fgets(head, 70, fp);
//	if (strncmp(head, "P6", 2)) {
//		fprintf(stderr, "%s: Not a raw PPM file\n", filename);
//	return NULL;
//	}

	// Grab the three elements in the header (width, height, maxval).
	i = 0;
	while (i < 3) {
		fgets( head, 70, fp );
		if ( head[0] == '#' )		// skip comments.
			continue;
		if ( i == 0 )
			i += sscanf( head, "%d %d %d", &w, &h, &d );
		else if ( i == 1 )
			i += sscanf( head, "%d %d", &h, &d );
		else if ( i == 2 )
			i += sscanf( head, "%d", &d );
	}

	// Grab all the image data in one fell swoop.
	image = (unsigned char*) malloc( sizeof( unsigned char ) * w * h * 3 );
	fread( image, sizeof( unsigned char ), w * h * 3, fp );
	fclose( fp );

	*width = w;
	*height = h;

	return image;
}
