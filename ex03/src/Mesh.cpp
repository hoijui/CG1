
#include "Mesh.h"

#include "ErrorDevel.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include "gl/glut.h"
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <cstdio>

Mesh::Mesh()
{
}

Mesh::Mesh(
		vector< vector<float> > vertices,
		int edges,
		vector< vector<int> > faces)
	: vertices(vertices)
	, edges(edges)
	, faces(faces)
{
}

Mesh::Mesh(const Mesh& other)
	: vertices(other.vertices)
	, edges(other.edges)
	, faces(other.faces)
{
}

Mesh Mesh::loadOff(const string& filePath) {

	FILE* fHandle = fopen(filePath.c_str(), "r");

	// read the header
	fscanf(fHandle, "OFF\n");
	int V, F, E;
	fscanf(fHandle, "%d %d %d\n", &V, &F, &E);

	// read the vertices
	vector< vector<float> > vertices;
	for (int v=0; v < V; ++v) {
		vector<float> vertex;
		float x, y, z;
		fscanf(fHandle, "%f %f %f\n", &x, &y, &z);
		vertex.push_back(x);
		vertex.push_back(y);
		vertex.push_back(z);
		vertices.push_back(vertex);
	}

	// read the polygons
	vector< vector<int> > polygons;
	char poygonCStr[1024];
	for (int v=0; v < V; ++v) {
		vector<int> polygon;
		fgets(poygonCStr, 1024, fHandle); // reads one line
		string poygonStr = poygonCStr;
		std::size_t curStart = 0;
		curStart = poygonStr.find_first_not_of(" ", curStart);
		std::size_t nextStart;
		nextStart = poygonStr.find_first_of(" ", curStart);
		// number of edges of the polygon
		int n = atoi(poygonStr.substr(curStart, nextStart - curStart).c_str());
		for (int i=0; i < n; ++i) {
			curStart = nextStart;
			curStart = poygonStr.find_first_not_of(" ", curStart);
			nextStart = poygonStr.find_first_of(" ", curStart);
			int verticeIndex = atoi(poygonStr.substr(curStart, nextStart - curStart).c_str());
			polygon.push_back(verticeIndex);
		}
		polygons.push_back(polygon);
	}

	return Mesh(vertices, E, polygons);
}


void Mesh::Display() const {

	glPushMatrix();
	for (int t = 0; t < faces.size(); ++t) {
		//cout << "draw face" << endl;
		const vector<int>& vertexIndices = faces.at(t);
		glBegin(GL_TRIANGLES); // HACK only works if verticeIndices.size() == 3
		for (int v = 0; v < vertexIndices.size(); ++v) {
			const vector<float>& vertex = vertices.at(vertexIndices.at(v));
			glVertex3f(vertex.at(0) / 1.0f, vertex.at(1) / 1.0f, vertex.at(2) / 1.0f);
		}
		glEnd();
	}
	glPopMatrix();
}
