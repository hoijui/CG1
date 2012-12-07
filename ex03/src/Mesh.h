
#ifndef _MESH_H_
#define _MESH_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

class Mesh {
private:
	Mesh(
			vector< vector<float> > vertices,
			int edges,
			vector< vector<int> > faces);

public:
	Mesh();
	Mesh(const Mesh& other);

	//! Reads meshes in the OFF file format
	static Mesh loadOff(const string& filePath);

	const vector< vector<float> >& GetVertices() const { return vertices; }
	int GetEdges() const { return edges; }
	const vector< vector<int> >& GetFaces() const { return faces; }

	void Display() const;

private:
	vector< vector<float> > vertices;
	int edges;
	vector< vector<int> > faces;
};

#endif // _MESH_H_
