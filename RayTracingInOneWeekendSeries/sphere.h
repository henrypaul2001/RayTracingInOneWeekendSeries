#pragma once
#include "hittable.h"
class sphere : public hittable {
public:
	// Stationary sphere
	sphere(const point3& static_center, const float radius, shared_ptr<material> mat) : center(static_center, vec3(0.0f)), radius(std::fmax(0.0f, radius)), mat(mat) {
		vec3 rvec = vec3(radius);

		bbox = aabb(static_center - rvec, static_center + rvec);
	}

	// Moving sphere
	sphere(const point3& center1, const point3& center2, const float radius, shared_ptr<material> mat) : center(center1, center2 - center1), radius(std::fmax(0.0f, radius)), mat(mat) {
		vec3 rvec = vec3(radius);
		aabb box1 = aabb(center.at(0) - rvec, center.at(0) + rvec);
		aabb box2 = aabb(center.at(1) - rvec, center.at(1) + rvec);
		bbox = aabb(box1, box2);
	}

	bool hit(const ray& r, const interval ray_t, hit_record& out_hit) const override {
		point3 current_center = center.at(r.time());
		vec3 oc = current_center - r.Origin();
		float a = r.Direction().length2();
		float h = dot(r.Direction(), oc);
		float c = oc.length2() - radius * radius;

		float discriminant = h * h - a * c;
		if (discriminant < 0) {
			return false;
		}

		float sqrtd = std::sqrt(discriminant);

		// Find the nearest root that lies in tmin, tmax range
		float root = (h - sqrtd) / a;
		if (!ray_t.surrounds(root)) {
			root = (h + sqrtd) / a;
			if (!ray_t.surrounds(root)) {
				return false;
			}
		}

		out_hit.t = root;
		out_hit.p = r.at(out_hit.t);
		vec3 outward_normal = (out_hit.p - current_center) / radius;
		out_hit.set_face_normal(r, outward_normal);
		out_hit.mat = mat;

		return true;
	}

	aabb bounding_box() const override { return bbox; }

private:
	ray center; // Acts as a path for animated spheres
	float radius;
	shared_ptr<material> mat;
	aabb bbox;
};