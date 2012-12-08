
#include "Mesh.h"

#include "Vec3f.h"

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
	: smooth(true)
	, surfaceDependentNormalWeighting(true)
{
}

Mesh::Mesh(
		vector<Vec3f> vertices,
		int edges,
		vector< vector<int> > faces)
	: vertices(vertices)
	, edges(edges)
	, faces(faces)
	, smooth(true)
	, surfaceDependentNormalWeighting(true)
{
	CalculateNormals();
}

Mesh::Mesh(const Mesh& other)
	: vertices(other.vertices)
	, edges(other.edges)
	, faces(other.faces)
	, faceNormals(other.faceNormals)
	, smooth(true)
	, surfaceDependentNormalWeighting(true)
{
}

Mesh Mesh::loadOff(const string& filePath) {

	FILE* fHandle = fopen(filePath.c_str(), "r");

	// read the header
	fscanf(fHandle, "OFF\n");
	int V, F, E;
	fscanf(fHandle, "%d %d %d\n", &V, &F, &E);

	// read the vertices
	vector<Vec3f> vertices;
	for (int v=0; v < V; ++v) {
		float x, y, z;
		fscanf(fHandle, "%f %f %f\n", &x, &y, &z);
		vertices.push_back(Vec3f(x, y, z));
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

void Mesh::CalculateNormals() {

	CalculateFaceNormals();
	CalculateVertexNormals();
}

void Mesh::CalculateFaceNormals() {

	faceNormals.clear();

	const Vec3f fixedNormalTest = Vec3f(1.0f, 1.0f, 1.0f).normalize();
	for (int t = 0; t < faces.size(); ++t) {
		const vector<int>& vertexIndices = faces.at(t);
		const Vec3f& vertex1 = vertices.at(vertexIndices.at(0));
		const Vec3f& vertex2 = vertices.at(vertexIndices.at(1));
		const Vec3f& flatNormal = vertex1.cross(vertex2).normalize();
		faceNormals.push_back(flatNormal);
		//faceNormals.push_back(fixedNormalTest); // HACK
	}
}

void Mesh::CalculateVertexNormals() {

	vertexNormals.clear();

	// fill with empty (0, 0, 0) normals
	for (int v = 0; v < vertices.size(); ++v) {
		vertexNormals.push_back(Vec3f(0.0f, 0.0f, 0.0f));
	}

	// to each vertex, add the 3 normals of the 3 adjacent triangles (and normalize)
	for (int t = 0; t < faces.size(); ++t) {
		const vector<int>& vertexIndices = faces.at(t);
		const int vInd0 = vertexIndices.at(0);
		const int vInd1 = vertexIndices.at(1);
		const int vInd2 = vertexIndices.at(2);
		const Vec3f& v0 = vertices.at(vInd0);
		const Vec3f& v1 = vertices.at(vInd1);
		const Vec3f& v2 = vertices.at(vInd2);

		// NOTE Alternatively, instead of weighting each of the 3 normals
		//   equally, we could
		if (surfaceDependentNormalWeighting) {
			vertexNormals.at(vInd0) += v2.cross(v0);
			vertexNormals.at(vInd1) += v0.cross(v1);
			vertexNormals.at(vInd2) += v1.cross(v2);
		} else {
			// each face has an equal weight
			vertexNormals.at(vInd0) += v2.cross(v0).normalize() / 3.0f;
			vertexNormals.at(vInd1) += v0.cross(v1).normalize() / 3.0f;
			vertexNormals.at(vInd2) += v1.cross(v2).normalize() / 3.0f;
		}
	}

	if (surfaceDependentNormalWeighting) {
		for (int v = 0; v < vertices.size(); ++v) {
			vertices.at(v).normalize();
		}
	}
}

void Mesh::SetSmoothRendering(bool enabled) {
	this->smooth = enabled;
}

void Mesh::Display() const {

	if (smooth) {
		renderSmooth();
	} else {
		renderFlat();
	}
}

void Mesh::renderFlat() const {

	const float SCALE_FACTOR = 1.0f / 100.0f; // HACK without this, the bunny is too big

	glPushMatrix();
	glScalef(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);
	for (int t = 0; t < faces.size(); ++t) {
		const vector<int>& vertexIndices = faces.at(t);
		const Vec3f& normal = faceNormals.at(t);
		glNormal3f(normal.GetX(), normal.GetY(), normal.GetZ());
		glBegin(GL_TRIANGLES); // HACK only works if verticeIndices.size() == 3
		for (int v = 0; v < vertexIndices.size(); ++v) {
			const Vec3f& vertex = vertices.at(vertexIndices.at(v));
			glVertex3f(vertex.GetX(), vertex.GetY(), vertex.GetZ());
		}
		glEnd();
	}
	glPopMatrix();
}

void Mesh::renderSmooth() const {

	const float SCALE_FACTOR = 1.0f / 100.0f; // HACK without this, the bunny is too big

	glPushMatrix();
	glScalef(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);
	for (int t = 0; t < faces.size(); ++t) {
		const vector<int>& vertexIndices = faces.at(t);
		glBegin(GL_TRIANGLES); // HACK only works if verticeIndices.size() == 3
		for (int v = 0; v < vertexIndices.size(); ++v) {
			const int vInd = vertexIndices.at(v);
			const Vec3f& vertex = vertices.at(vInd);
			const Vec3f& normal = vertexNormals.at(vInd);
			glNormal3f(normal.GetX(), normal.GetY(), normal.GetZ());
			glVertex3f(vertex.GetX(), vertex.GetY(), vertex.GetZ());
		}
		glEnd();
	}
	glPopMatrix();
}