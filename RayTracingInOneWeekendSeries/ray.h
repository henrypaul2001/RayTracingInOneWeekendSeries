#pragma once
#include "vec3.h"

class ray {
public:
	ray() {}
	ray(const point3& origin, const vec3& direction) : origin(origin), direction(direction) {}

	const point3& Origin() const { return origin; }
	const vec3& Direction() const { return direction; }

	point3 at(float t) const {
		return origin + (t * direction);
	}

private:
	point3 origin;
	vec3 direction;
};