
#include "Box.h"

/**
 * Tests intersection with a ray.
 * @see http://people.csail.mit.edu/amy/papers/box-jgt.pdf
 */
bool Box::TestIntersection(const Ray& r, float t0, float t1) const {

	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[r.sign[0]].x - r.o.x) * r.inv_d.x;
	tmax = (bounds[1-r.sign[0]].x - r.o.x) * r.inv_d.x;
	tymin = (bounds[r.sign[1]].y - r.o.y) * r.inv_d.y;
	tymax = (bounds[1-r.sign[1]].y - r.o.y) * r.inv_d.y;

	if ( (tmin > tymax) || (tymin > tmax) ) {
		return false;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}

	tzmin = (bounds[r.sign[2]].z - r.o.z) * r.inv_d.z;
	tzmax = (bounds[1-r.sign[2]].z - r.o.z) * r.inv_d.z;

	if ( (tmin > tzmax) || (tzmin > tmax) ) {
		return false;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}

	return ( (tmin < t1) && (tmax > t0) );
}
