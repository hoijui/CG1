
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

		void RenderFaces() const;
		void renderFlat() const;
		void renderSmooth() const;

		void RenderNormals() const;
		void RenderFaceNormals() const;
		void RenderVertexNormals() const;

		static void RenderNormal(const Vec3f& position, const Vec3f& direction);

		const vector<Vec3f>& GetVertices() const { return vertices; }
		int GetEdges() const { return edges; }
		const vector< vector<int> >& GetFaces() const { return faces; }

	public:
		Mesh();
		Mesh(const Mesh& other);

		//! Reads meshes in the OFF file format
		static Mesh loadOff(const string& filePath);

		void SaveOff(const string& filePath) const;

		//! Moves the object, so the point mean is at (0, 0, 0)
		void Center();
		//! Scales the object, so the average distance from (0, 0, 0) is one
		void StdDist();

		void SetSmoothRendering(bool enabled);

		bool GetRenderNormals() const;
		void SetRenderNormals(bool enabled);

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
		bool renderNormals;
};

#endif // _MESH_H_
