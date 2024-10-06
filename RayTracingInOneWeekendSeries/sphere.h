#pragma once
#include "hittable.h"
class sphere : public hittable {
public:
	sphere(const point3& center, const float radius, shared_ptr<material> mat) : center(center), radius(std::fmax(0.0f, radius)), mat(mat) {}

	bool hit(const ray& r, const interval ray_t, hit_record& out_hit) const override {
		vec3 oc = center - r.Origin();
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
		vec3 outward_normal = (out_hit.p - center) / radius;
		out_hit.set_face_normal(r, outward_normal);
		out_hit.mat = mat;

		return true;
	}

private:
	point3 center;
	float radius;
	shared_ptr<material> mat;
};