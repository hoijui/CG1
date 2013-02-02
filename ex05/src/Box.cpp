
#include "Box.h"

/**
 * Tests intersection with a ray.
 * Optimized method. see:
 * @see http://people.csail.mit.edu/amy/papers/box-jgt.pdf
 */
bool Box::TestIntersection(const Ray& r, float t0, float t1) const {

	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[r.sign[0]].x      - r.o.x) * r.inv_d.x;
	tmax = (bounds[1 - r.sign[0]].x  - r.o.x) * r.inv_d.x;

	tymin = (bounds[r.sign[1]].y     - r.o.y) * r.inv_d.y;
	tymax = (bounds[1 - r.sign[1]].y - r.o.y) * r.inv_d.y;
	if ((tmin > tymax) || (tymin > tmax)) {
		return false;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}

	tzmin = (bounds[r.sign[2]].z     - r.o.z) * r.inv_d.z;
	tzmax = (bounds[1 - r.sign[2]].z - r.o.z) * r.inv_d.z;
	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}

	return ((tmin < t1) && (tmax > t0));
}

///**
// * Tests intersection with a ray.
// * Smits’ method (slower/more problems in special cases).
// */
//bool Box::TestIntersection(const Ray& r, float t0, float t1) const {

//	float tmin, tmax, tymin, tymax, tzmin, tzmax;

//	if (r.d.x >= 0) {
//		tmin = (bounds[0].x - r.o.x) / r.d.x;
//		tmax = (bounds[1].x - r.o.x) / r.d.x;
//	} else {
//		tmin = (bounds[1].x - r.o.x) / r.d.x;
//		tmax = (bounds[0].x - r.o.x) / r.d.x;
//	}

//	if (r.d.y >= 0) {
//		tymin = (bounds[0].y - r.o.y) / r.d.y;
//		tymax = (bounds[1].y - r.o.y) / r.d.y;
//	} else {
//		tymin = (bounds[1].y - r.o.y) / r.d.y;
//		tymax = (bounds[0].y - r.o.y) / r.d.y;
//	}
//	if ((tmin > tymax) || (tymin > tmax)) {
//		return false;
//	}
//	if (tymin > tmin) {
//		tmin = tymin;
//	}
//	if (tymax < tmax) {
//		tmax = tymax;
//	}

//	if (r.d.z >= 0) {
//		tzmin = (bounds[0].z - r.o.z) / r.d.z;
//		tzmax = (bounds[1].z - r.o.z) / r.d.z;
//	} else {
//		tzmin = (bounds[1].z - r.o.z) / r.d.z;
//		tzmax = (bounds[0].z - r.o.z) / r.d.z;
//	}
//	if ((tmin > tzmax) || (tzmin > tmax)) {
//		return false;
//	}
//	if (tzmin > tmin) {
//		tmin = tzmin;
//	}
//	if (tzmax < tmax) {
//		tmax = tzmax;
//	}

//	return ((tmin < t1) && (tmax > t0));
//}
