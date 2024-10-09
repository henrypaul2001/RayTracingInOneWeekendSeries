#pragma once
#include "rtweekend.h"
#include "aabb.h"

class material;

struct hit_record {
	point3 p;
	vec3 normal;
	shared_ptr<material> mat;
	float t;
	float u;
	float v;
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
	virtual bool hit(const ray& r, const interval ray_t, hit_record& out_hit) const = 0;
	virtual aabb bounding_box() const = 0;
};