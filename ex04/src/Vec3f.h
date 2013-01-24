#ifndef _VEC_3F_H_
#define _VEC_3F_H_

#include <cmath>

class Vec3f
{
	public:
		Vec3f();
		Vec3f(const Vec3f& other);
		Vec3f(float x, float y, float z);

		void operator+=(const Vec3f& other) {
			x += other.GetX();
			y += other.GetY();
			z += other.GetZ();
		}
		void operator-=(const Vec3f& other) {
			x -= other.GetX();
			y -= other.GetY();
			z -= other.GetZ();
		}
		void operator+=(const float scalar) {
			x += scalar;
			y += scalar;
			z += scalar;
		}
		void operator-=(const float scalar) {
			x -= scalar;
			y -= scalar;
			z -= scalar;
		}
		void operator*=(const float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
		}
		void operator/=(const float scalar) {
			x /= scalar;
			y /= scalar;
			z /= scalar;
		}

		Vec3f operator+(const Vec3f& other) const {
			return Vec3f(x + other.GetX(), y + other.GetY(),  z + other.GetZ());
		}
		Vec3f operator-(const Vec3f& other) const {
			return Vec3f(x - other.GetX(), y - other.GetY(),  z - other.GetZ());
		}
		Vec3f operator+(const float scalar) const {
			return Vec3f(x + scalar, y + scalar,  z + scalar);
		}
		Vec3f operator-(const float scalar) const {
			return Vec3f(x - scalar, y - scalar,  z - scalar);
		}
		Vec3f operator*(const float scalar) const {
			return Vec3f(x * scalar, y * scalar,  z * scalar);
		}
		Vec3f operator/(const float scalar) const {
			return Vec3f(x / scalar, y / scalar,  z / scalar);
		}

		float normSquared() const {
			return (x * x + y * y +  z * z);
		}
		float norm() const {
			return sqrt(normSquared());
		}
		Vec3f normalize() {
			return *this * (1.0f / norm());
		}

		Vec3f cross(const Vec3f& other) const {
			return Vec3f(
					y*other.z - z*other.y,
					z*other.x - x*other.z,
					x*other.y - y*other.x);
		}

		float GetX() const { return x; }
		float GetY() const { return y; }
		float GetZ() const { return z; }

		void SetX(const float x) { this->x = x; }
		void SetY(const float y) { this->y = y; }
		void SetZ(const float z) { this->z = z; }

	private:
		float x, y, z;
};

#endif // _VEC_3F_H_
