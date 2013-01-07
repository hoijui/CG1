#include "Vec3f.h"

Vec3f::Vec3f(const Vec3f& other)
	: x(other.x)
	, y(other.y)
	, z(other.z)
{
}

Vec3f::Vec3f(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
{
}
