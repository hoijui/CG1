
#ifndef _SCENE_H_
#define _SCENE_H_

#include "Box.h"
#include "Ray.h"
#include "Mesh.h"

#include <string>
#include <vector>

#include <glm/glm.hpp>

using namespace glm;
using std::string;
using std::vector;

class Scene {
	public:
		Scene();
		Scene(const Scene& other);

		void AddMesh(const string& fileName, const vec3& position, const float resizeFactor, const vec3& color);

		bool GetIntersectionPos(const Ray& r, float& out_t, vec3* out_color = NULL, vec3* out_normal = NULL) const;

		void Display() const;

	private:
		void RenderBox(const Box& box) const;
		static Box CalculateBoundingBox(const Mesh& mesh, const vec3& pos);
		static bool IntersectTriangle(const Ray& r, const vec3& v1, const vec3& v2, const vec3& v3, float& t);

		vector<Mesh> meshes;
		vector<vec3> positions;
		vector<Box> boundingBoxes;
		vector<vec3> colors;
};

#endif // _SCENE_H_
