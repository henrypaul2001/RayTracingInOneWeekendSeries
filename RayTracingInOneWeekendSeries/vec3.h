#pragma once
#include <iostream>
class vec3
{
public:
	float e[3];

	vec3() : e{ 0.0f, 0.0f, 0.0f } {}
	vec3(const float xyz) : e{ xyz, xyz, xyz } {}
	vec3(const float x, const float y, const float z) : e{x, y, z} {}

	float x() const { return e[0]; }
	float y() const { return e[1]; }
	float z() const { return e[2]; }

	float r() const { return e[0]; }
	float g() const { return e[1]; }
	float b() const { return e[2]; }

	vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	float operator[](int i) const { return e[i]; }
	float& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v) {
		e[0] += v.e[0];
		e[1] += v.e[1];
		e[2] += v.e[2];
		return *this;
	}

	vec3& operator*=(float t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}

	vec3& operator/=(float t) {
		return *this *= 1 / t;
	}

	float length2() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
	float length() const { return std::sqrt(length2()); }

	bool near_zero() const {
		// Return true if vector is close to zero in all dimensions
		auto s = 1e-8;
		return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
	}

	static vec3 random() { return vec3(random_double(), random_double(), random_double()); }
	static vec3 random(const float min, const float max) { return vec3(random_double(min, max), random_double(min, max), random_double(min, max)); }
};

// Aliases
using point3 = vec3;

// Utility functions
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(float t, const vec3& v) {
	return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

inline vec3 operator*(const vec3& v, float t) {
	return t * v;
}

inline vec3 operator/(const vec3& v, float t) {
	return (1 / t) * v;
}

inline float dot(const vec3& u, const vec3& v) {
	return u.e[0] * v.e[0]
		 + u.e[1] * v.e[1]
		 + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
				u.e[2] * v.e[0] - u.e[0] * v.e[2],
				u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 normalize(const vec3& v) {
	return v / v.length();
}

inline vec3 random_unit_vector() {
	while (true) {
		vec3 p = vec3::random(-1.0f, 1.0f);
		double length2 = p.length2();
		if (1e-160 < length2 && length2 <= 1.0) {
			return p / sqrt(length2);
		}
	}
}

inline vec3 random_on_hemisphere(const vec3& normal) {
	vec3 on_unit_sphere = random_unit_vector();
	if (dot(on_unit_sphere, normal) > 0.0f) {
		return on_unit_sphere;
	}
	else {
		return -on_unit_sphere;
	}
}

inline vec3 lerp(const vec3& start, const vec3& end, float t) {
	return (1.0f - t) * start + t * end;
}