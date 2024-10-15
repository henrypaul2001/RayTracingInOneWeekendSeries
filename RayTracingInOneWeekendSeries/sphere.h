#pragma once
#include "hittable.h"
#include "onb.h"
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
		get_sphere_uv(outward_normal, out_hit.u, out_hit.v);
		out_hit.mat = mat;

		return true;
	}

	aabb bounding_box() const override { return bbox; }

	float pdf_value(const point3& origin, const vec3& direction) const override {
		// This method only works for stationary spheres
		hit_record rec;
		if (!this->hit(ray(origin, direction), interval(0.001f, infinity), rec)) {
			return 0.0f;
		}

		float dist_squared = (center.at(0.0f) - origin).length2();
		float cos_theta_max = std::sqrt(1 - radius * radius / dist_squared);
		float solid_angle = 2.0f * pi * (1.0f - cos_theta_max);

		return 1.0f / solid_angle;
	}

	vec3 random(const point3& origin) const override {
		vec3 direction = center.at(0.0f) - origin;
		float distance_squared = direction.length2();
		onb uvw = onb(direction);
		return uvw.transform(random_to_sphere(radius, distance_squared));
	}

private:
	ray center; // Acts as a path for animated spheres
	float radius;
	shared_ptr<material> mat;
	aabb bbox;

	static vec3 random_to_sphere(float radius, float distance_squared) {
		float r1 = random_double();
		float r2 = random_double();
		float z = 1.0f + r2 * (std::sqrt(1.0f - radius * radius / distance_squared) - 1.0f);

		float phi = 2.0f * pi * r1;
		float x = std::cos(phi) * std::sqrt(1.0f - z * z);
		float y = std::sin(phi) * std::sqrt(1.0f - z * z);
		
		return vec3(x, y, z);
	}

	static void get_sphere_uv(const point3& p, float& u, float& v) {
		float theta = std::acos(-p.y());
		float phi = std::atan2(-p.z(), p.x()) + pi;

		u = phi / (2 * pi);
		v = theta / pi;
	}
};