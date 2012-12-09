
#ifndef _MESH_H_
#define _MESH_H_

#include "Vec3f.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

class Mesh {
private:
	Mesh(
			vector<Vec3f> vertices,
			int edges,
			vector< vector<int> > faces);

	void CalculateNormals();
	void CalculateFaceNormals();
	void CalculateVertexNormals();

	void renderFlat() const;
	void renderSmooth() const;

	const vector<Vec3f>& GetVertices() const { return vertices; }
	int GetEdges() const { return edges; }
	const vector< vector<int> >& GetFaces() const { return faces; }

public:
	Mesh();
	Mesh(const Mesh& other);

	//! Reads meshes in the OFF file format
	static Mesh loadOff(const string& filePath);

	//! Moves the object, so the point mean is at (0, 0, 0)
	void Center();
	//! Scales the object, so the average distance from (0, 0, 0) is one
	void StdDist();

	void SetSmoothRendering(bool enabled);

	void Display() const;

private:
	vector<Vec3f> vertices;
	int edges;
	vector< vector<int> > faces;
	/** One normalfaceNormalsFlat for flat shading */
	vector<Vec3f> faceNormals;
	/** One normal per vertex, used for smooth (Gouraud) shading */
	vector<Vec3f> vertexNormals;
	bool smooth;
	bool surfaceDependentNormalWeighting;
};

#endif // _MESH_H_
