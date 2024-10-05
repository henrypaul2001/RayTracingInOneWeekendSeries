#pragma once
#include "ray.h"

struct hit_record {
public:
	point3 p;
	vec3 normal;
	float t;
	bool front_face;

	void set_face_normal(const ray& r, const vec3& outward_normal) {
		// Sets the hit_record normal vector.
		// NOTE: the parameter outward_normal is assumed to be normalized

		front_face = dot(r.Direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
public:
	virtual ~hittable() = default;
	virtual bool hit(const ray& r, const float ray_tmin, const float ray_tmax, hit_record& out_hit) const = 0;
};