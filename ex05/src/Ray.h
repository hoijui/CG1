
#ifndef _RAY_H_
#define _RAY_H_

#include <glm/glm.hpp>

using namespace glm;

/*
 * A simple structure representing a ray.
 */
class Ray
{
public:
	vec3 o;
	vec3 d;
	vec3 inv_d; ///< for faster ray-box-intersection testing
	int sign[3]; ///< for faster ray-box-intersection testing

	float tmin;

//	Ray()
//		: tmin(0)
//	{
//	}
	Ray(const vec3& origin, const vec3& dir, float eps = 0.0f)
		: o(origin + eps*dir)
		, d(dir)
		, tmin(0)
	{
		inv_d = vec3(1.0f / d.x, 1.0f / d.y, 1.0f / d.z);
		inv_d = vec3(1.0f / d.x, 1.0f / d.y, 1.0f / d.z);
		sign[0] = (inv_d.x < 0.0f);
		sign[1] = (inv_d.y < 0.0f);
		sign[2] = (inv_d.z < 0.0f);
	}

	vec3 att(float t) const {
		return o + t*d;
	}
};

#endif // _RAY_H_
