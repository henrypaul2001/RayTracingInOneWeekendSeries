#pragma once
#include "ray.h"

struct hit_record {
public:
	point3 p;
	vec3 normal;
	float t;
};

class hittable {
public:
	virtual ~hittable() = default;
	virtual bool hit(const ray& r, const float ray_tmin, const float ray_tmax, hit_record& out_hit) const = 0;
};