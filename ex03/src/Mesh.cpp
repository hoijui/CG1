
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

	if (fHandle == NULL) {
		std::cout << " Failed to open file for reading: \"" << filePath << "\"" << std::endl;
		exit(99);
	}

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
	for (int f=0; f < F; ++f) {
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

void Mesh::SaveOff(const string& filePath) const {

	FILE* fHandle = fopen(filePath.c_str(), "w");

	if (fHandle == NULL) {
		std::cout << " Failed to open file for writing: \"" << filePath << "\"" << std::endl;
		exit(99);
	}

	// write the header
	fprintf(fHandle, "OFF\n");
	fprintf(fHandle, "%d %d %d\n", (int)vertices.size(), (int)faces.size(), 0);

	// write the vertices
	for (int v=0; v < vertices.size(); ++v) {
		const Vec3f& vert = vertices.at(v);
		fprintf(fHandle, "%f %f %f\n", vert.GetX(), vert.GetY(), vert.GetZ());
	}

	// write the polygons
	for (int f=0; f < faces.size(); ++f) {
		const vector<int>& face = faces.at(f);
		fprintf(fHandle, "%d", (int)face.size());
		for (int i=0; i < face.size(); ++i) {
			fprintf(fHandle, " %d", face.at(i));
		}
		fprintf(fHandle, "\n");
	}
}

void Mesh::Center() {

	Vec3f sum(0.0f, 0.0f, 0.0f);
	for (int v=0; v < vertices.size(); ++v) {
		sum += vertices.at(v);
	}
	const Vec3f center = sum / vertices.size();
	for (int v=0; v < vertices.size(); ++v) {
		vertices.at(v) -= center;
	}

	CalculateNormals();
}

void Mesh::StdDist() {

	double sumDists = 0.0;
	for (int v=0; v < vertices.size(); ++v) {
		sumDists += vertices.at(v).norm();
	}
	const float averageDist = sumDists / vertices.size();
	for (int v=0; v < vertices.size(); ++v) {
		vertices.at(v) /= averageDist;
	}

	CalculateNormals();
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
		const Vec3f& v0 = vertices.at(vertexIndices.at(0));
		const Vec3f& v1 = vertices.at(vertexIndices.at(1));
		const Vec3f& v2 = vertices.at(vertexIndices.at(2));
		const Vec3f& edge1 = v1 - v0;
		const Vec3f& edge3 = v0 - v2;
		const Vec3f& flatNormal = edge3.cross(edge1).normalize();
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
		const Vec3f& e0 = v1 - v0;
		const Vec3f& e1 = v2 - v1;
		const Vec3f& e2 = v0 - v2;

		// NOTE Alternatively, instead of weighting each of the 3 normals
		//   equally, we could
		if (surfaceDependentNormalWeighting) {
			vertexNormals.at(vInd0) += e2.cross(e0);
			vertexNormals.at(vInd1) += e0.cross(e1);
			vertexNormals.at(vInd2) += e1.cross(e2);
		} else {
			// each face has an equal weight
			vertexNormals.at(vInd0) += e2.cross(e0).normalize() / 3.0f;
			vertexNormals.at(vInd1) += e0.cross(e1).normalize() / 3.0f;
			vertexNormals.at(vInd2) += e1.cross(e2).normalize() / 3.0f;
		}
	}

	if (surfaceDependentNormalWeighting) {
		for (int v = 0; v < vertices.size(); ++v) {
			vertexNormals.at(v) = vertexNormals.at(v).normalize();
		}
	}
}

void Mesh::SetSmoothRendering(bool enabled) {
	this->smooth = enabled;
}

void Mesh::Display() const {

	RenderFaces();
	RenderNormals();
}

void Mesh::RenderFaces() const {

	if (smooth) {
		renderSmooth();
	} else {
		renderFlat();
	}
}

void Mesh::renderFlat() const {

	glPushMatrix();
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

	glPushMatrix();
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

void Mesh::RenderNormals() const {

	glColor3f(1.0f, 0.0f, 0.0f);
	if (smooth) {
		RenderVertexNormals();
	} else {
		RenderFaceNormals();
	}
	glColor3f(1.0f, 1.0f, 1.0f);
}

void Mesh::RenderFaceNormals() const {

	glPushMatrix();
	for (int t = 0; t < faces.size(); ++t) {
		const vector<int>& vertexIndices = faces.at(t);
		const Vec3f& normal = faceNormals.at(t);
		Vec3f faceCenter(0.0f, 0.0f, 0.0f);
		for (int v = 0; v < vertexIndices.size(); ++v) {
			faceCenter += vertices.at(vertexIndices.at(v));
		}
		faceCenter /= vertexIndices.size();
		RenderNormal(faceCenter, normal / 5.0f);
	}
	glPopMatrix();
}

void Mesh::RenderVertexNormals() const {

	glPushMatrix();
	for (int t = 0; t < faces.size(); ++t) {
		const vector<int>& vertexIndices = faces.at(t);
		for (int v = 0; v < vertexIndices.size(); ++v) {
			const int vInd = vertexIndices.at(v);
			const Vec3f& vertex = vertices.at(vInd);
			const Vec3f& normal = vertexNormals.at(vInd);
			RenderNormal(vertex, normal / 5.0f);
		}
	}
	glPopMatrix();
}

void Mesh::RenderNormal(const Vec3f& position, const Vec3f& direction) {

	glBegin(GL_LINES);
	glVertex3f(position.GetX(), position.GetY(), position.GetZ());
	glVertex3f(position.GetX() + direction.GetX(), position.GetY() + direction.GetY(), position.GetZ() + direction.GetZ());
	glEnd();
}
