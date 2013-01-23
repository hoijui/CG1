/* ----------------------------------------------------------------
name:           Image.hpp
purpose:        texturing tutorial
'introduction to computer graphics'
winter term 2012/2013, assignment 4
version:	   SKELETON CODE
TODO:           nothing (see Image.cpp)
author:         katrin lang
computer graphics
tu berlin
------------------------------------------------------------- */

#pragma once

#ifdef __APPLE__
#include <GLUT/freeglut.h>
#elif _WIN32
#include "win32/freeglut.h"
#else
#include <GL/freeglut.h>
#endif

#include <vector>
#include <string>

#include "glm/glm.hpp"

class Image {

	public:
		// constructors
		Image();
		Image(int width, int height);
		Image(const std::string& filename);

		/// destructor
		~Image();

		/// load image from file
		void load(const std::string& filename);

		/// set modulation
		void setModulation(GLuint modulation);
		/// set texture filter
		void setMinFilter(GLuint min);
		void setMagFilter(GLuint mag);

		/// bind/unbind texture
		void bind();
		void unbind();

		/// generate OpenGL texture
		void generateTexture();

		/// draw in texture
		void paint(int x, int y);

		/// erase drawing from texture
		void erase(int x, int y);

		GLuint textureID;

	protected:
		// image data
		std::vector<glm::vec4> data;
		std::vector<glm::vec4> originalData;
		// dimensions
		int width;
		int height;


		// texturing parameters
		GLuint wrap;
		GLuint mag;
		GLuint min;
		GLuint modulate;

		/// read a pixel from image
		glm::vec4 get(int x, int y);

		/// parse ppm format
		void loadPPM(const std::string& filename);

		/// parse ppm again
		unsigned char* loadPPMToChar(const char* filename, int* width, int* height );

		GLubyte* ttexture;
};
