#pragma once

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
public:
	constant_medium(shared_ptr<hittable> boundary, const float density, shared_ptr<texture> tex) : boundary(boundary), neg_inv_density(-1.0f / density), phase_function(make_shared<isotropic>(tex)) {}
	constant_medium(shared_ptr<hittable> boundary, const float density, const colour& albedo) : boundary(boundary), neg_inv_density(-1.0f / density), phase_function(make_shared<isotropic>(albedo)) {}

	bool hit(const ray& r, const interval ray_t, hit_record& out_hit) const override {
		return false; // not yet implemented
	}

	aabb bounding_box() const override { return boundary->bounding_box(); }

private:
	shared_ptr<hittable> boundary;
	float neg_inv_density;
	shared_ptr<material> phase_function;
};