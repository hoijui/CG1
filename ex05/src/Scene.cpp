
#include "Scene.h"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <limits>

#ifdef __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include "gl/glut.h"
#else
#include <GL/glut.h>
#endif

// HACK
//static float zNear = 0.1;
//static float zFar = 100.0;
// HACK HACK
static float zNear = -10000.f;
static float zFar = 10000.f;

Scene::Scene()
{
}

Scene::Scene(const Scene& other)
	: meshes(other.meshes)
	, positions(other.positions)
	, boundingBoxes(other.boundingBoxes)
{
}

void Scene::AddMesh(const string& fileName, const vec3& position, const float resizeFactor, const vec3& color) {

	Mesh newMesh = Mesh::loadOff(fileName);
	newMesh.Center();
	newMesh.StdDist();
	newMesh.Resize(resizeFactor);

	Box boundingBox = CalculateBoundingBox(newMesh, position);

	meshes.push_back(newMesh);
	positions.push_back(position);
	boundingBoxes.push_back(boundingBox);
	colors.push_back(color);
}

bool Scene::GetIntersectionPos(const Ray& r, float& out_t, vec3& out_color, vec3* out_normal) const {

	// check whichs meshes bounding boxes are intersected by the ray
	vector<int> intersectingBoxes;
	for (int meshIndex = 0; meshIndex < boundingBoxes.size(); ++meshIndex) {
		if (boundingBoxes.at(meshIndex).TestIntersection(r, zNear, zFar)) {
			intersectingBoxes.push_back(meshIndex);
		}
	}

	// now check only these meshes for intersection
	int minMeshIndex = -1;
	int minFaceIndex = -1;
	float minT = __FLT_MAX__;
	float t;
	for (vector<int>::const_iterator boxIt = intersectingBoxes.begin(); boxIt != intersectingBoxes.end(); ++boxIt) {
		const int meshIndex = *boxIt;
		const Mesh& mesh = meshes.at(meshIndex);
		const vector< vector<int> >& faces = mesh.GetFaces();
		const vector<Vec3f>& vertices = mesh.GetVertices();
		Ray localRay(r);
		localRay.o -= positions.at(meshIndex);
		for (int faceIndex = 0; faceIndex < faces.size(); ++faceIndex) {
			const vector<int>& face = faces.at(faceIndex);
			const Vec3f& v0 = vertices.at(face.at(0));
			const Vec3f& v1 = vertices.at(face.at(1));
			const Vec3f& v2 = vertices.at(face.at(2));
			if (IntersectTriangle(localRay, *((vec3*)&v0), *((vec3*)&v1), *((vec3*)&v2), t) && (t < minT)) {
				minMeshIndex = meshIndex;
				minFaceIndex = faceIndex;
				minT = t;
			}
		}
	}

	const bool intersected = (minMeshIndex >= 0);

	if (intersected) {
		out_t = minT;

		if (out_normal != NULL) {
			const Mesh& mesh = meshes.at(minMeshIndex);
			const vec3& meshPos = positions.at(minMeshIndex);
			const vector< vector<int> >& faces = mesh.GetFaces();
			const vector<int>& face = faces.at(minFaceIndex);
			const vector<Vec3f>& vertices = mesh.GetVertices();
			const vector<Vec3f>& vertexNormals = mesh.GetVertexNormals();

			vec3 intersectionPos = r.att(minT); // scene global
			intersectionPos -= meshPos; // mesh local
			const Vec3f intPosLoc = Vec3f(*((Vec3f*)&intersectionPos));
			const float facN0 = 1.0f / (vertices.at(0) - intPosLoc).norm();
			const float facN1 = 1.0f / (vertices.at(1) - intPosLoc).norm();
			const float facN2 = 1.0f / (vertices.at(2) - intPosLoc).norm();

			Vec3f interpolatedNormal
					= (vertexNormals.at(0) * facN0)
					+ (vertexNormals.at(1) * facN1)
					+ (vertexNormals.at(2) * facN2);
			interpolatedNormal * 3.0f;
			interpolatedNormal.normalize();

			*out_normal = *((vec3*)&interpolatedNormal);
		}
	}

	out_color = vec3(1.0f, 0.0f, 0.0f);

	return intersected;
}

void Scene::Display() const {

	for (int meshIndex = 0; meshIndex < meshes.size(); ++meshIndex) {
		glPushMatrix();
		glColor3f(1.0f, 1.0f, 0.0f);
		RenderBox(boundingBoxes.at(meshIndex));

		const vec3& pos = positions.at(meshIndex);
		const vec3& color = colors.at(meshIndex);
		glColor3fv(&color[0]);
		glTranslatef(pos.x, pos.y, pos.z);
		meshes.at(meshIndex).Display();
		glColor3f(1.0f, 1.0f, 1.0f);
		glPopMatrix();
	}
}

void Scene::RenderBox(const Box& box) const {

	const float xMin = box.bounds[0].x;
	const float yMin = box.bounds[0].y;
	const float zMin = box.bounds[0].z;
	const float xMax = box.bounds[1].x;
	const float yMax = box.bounds[1].y;
	const float zMax = box.bounds[1].z;

	glBegin(GL_LINES);
	glVertex3f(xMin, yMin, zMin); glVertex3f(xMin, yMin, zMax);
	glVertex3f(xMin, yMin, zMin); glVertex3f(xMin, yMax, zMin);
	glVertex3f(xMin, yMin, zMin); glVertex3f(xMax, yMin, zMin);

	glVertex3f(xMin, yMin, zMax); glVertex3f(xMin, yMax, zMax);
	glVertex3f(xMin, yMin, zMax); glVertex3f(xMax, yMin, zMax);

	glVertex3f(xMin, yMax, zMin); glVertex3f(xMin, yMax, zMax);
	glVertex3f(xMin, yMax, zMin); glVertex3f(xMax, yMax, zMin);

	glVertex3f(xMax, yMin, zMin); glVertex3f(xMax, yMin, zMax);
	glVertex3f(xMax, yMin, zMin); glVertex3f(xMax, yMax, zMin);

	glVertex3f(xMax, yMax, zMax); glVertex3f(xMax, yMax, zMin);
	glVertex3f(xMax, yMax, zMax); glVertex3f(xMax, yMin, zMax);
	glVertex3f(xMax, yMax, zMax); glVertex3f(xMin, yMax, zMax);
	glEnd();
}

Box Scene::CalculateBoundingBox(const Mesh& mesh, const vec3& pos) {

	vec3 bbMin(__FLT_MAX__, __FLT_MAX__, __FLT_MAX__);
	vec3 bbMax(-__FLT_MAX__, -__FLT_MAX__, -__FLT_MAX__);

	const vector<Vec3f>& vertices = mesh.GetVertices();
	for (vector<Vec3f>::const_iterator vertIt = vertices.begin(); vertIt != vertices.end(); ++vertIt) {
		if (vertIt->GetX() < bbMin.x) {
			bbMin.x = vertIt->GetX();
		}
		if (vertIt->GetX() > bbMax.x) {
			bbMax.x = vertIt->GetX();
		}

		if (vertIt->GetY() < bbMin.y) {
			bbMin.y = vertIt->GetY();
		}
		if (vertIt->GetY() > bbMax.y) {
			bbMax.y = vertIt->GetY();
		}

		if (vertIt->GetZ() < bbMin.z) {
			bbMin.z = vertIt->GetZ();
		}
		if (vertIt->GetZ() > bbMax.z) {
			bbMax.z = vertIt->GetZ();
		}
	}

	bbMin += pos;
	bbMax += pos;

	return Box(bbMin, bbMax);
}

bool Scene::IntersectTriangle(const Ray& r, const vec3& v1, const vec3& v2, const vec3& v3, float& t)
{
	// see ray tracing slides p. 23
	// this is the Moeller-Trombore algorithm explained here:
	// http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-9-ray-triangle-intersection/m-ller-trumbore-algorithm/
	vec3 v2mv1 = v2 - v1;
	vec3 v3mv1 = v3 - v1;
	float det = dot(cross(r.d, v3mv1), v2mv1);
	float inv_det = 1.0f / det;
	t = inv_det * dot(cross(r.o - v1, v2mv1), v3mv1);
	const float u = inv_det * dot(cross(r.d, v3mv1), r.o - v1);
	const float v = inv_det * dot(cross(r.o - v1, v2mv1), r.d);
	const bool intersected = ((0.0f < t) && (0.0f < u) && (0.0f < v) && ((u + v) < 1));

	return intersected;
}
