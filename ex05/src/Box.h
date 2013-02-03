
#ifndef _BOX_H_
#define _BOX_H_

#include <glm/glm.hpp>

#include "Ray.h"

using namespace glm;

/**
 * A simple structure representing a box.
 */
class Box
{
public:
	vec3 bounds[2];

	Box(const Box& other);
	Box(const vec3& min, const vec3& max);

	bool TestIntersection(const Ray& r, float t0, float t1) const;
};

#endif // _BOX_H_
