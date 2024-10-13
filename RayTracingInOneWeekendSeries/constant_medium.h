#pragma once

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
public:
	constant_medium(shared_ptr<hittable> boundary, const float density, shared_ptr<texture> tex) : boundary(boundary), neg_inv_density(-1.0f / density), phase_function(make_shared<isotropic>(tex)) {}
	constant_medium(shared_ptr<hittable> boundary, const float density, const colour& albedo) : boundary(boundary), neg_inv_density(-1.0f / density), phase_function(make_shared<isotropic>(albedo)) {}

	bool hit(const ray& r, const interval ray_t, hit_record& out_hit) const override {
		hit_record rec1, rec2;

		if (!boundary->hit(r, interval::universe, rec1)) {
			return false;
		}

		if (!boundary->hit(r, interval(rec1.t + 0.0001f, infinity), rec2)) {
			return false;
		}

		if (rec1.t < ray_t.min) { rec1.t = ray_t.min; }
		if (rec2.t > ray_t.max) { rec2.t = ray_t.max; }

		if (rec1.t >= rec2.t) {
			return false;
		}

		if (rec1.t < 0.0f) {
			rec1.t = 0.0f;
		}

		float ray_length = r.Direction().length();
		float distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
		float hit_distance = neg_inv_density * std::log(fast_random_double());

		if (hit_distance > distance_inside_boundary) {
			return false;
		}

		out_hit.t = rec1.t + hit_distance / ray_length;
		out_hit.p = r.at(out_hit.t);

		out_hit.normal = vec3(1.0f, 0.0f, 0.0f);
		out_hit.front_face = true;
		out_hit.mat = phase_function;

		return true;
	}

	aabb bounding_box() const override { return boundary->bounding_box(); }

private:
	shared_ptr<hittable> boundary;
	float neg_inv_density;
	shared_ptr<material> phase_function;
};