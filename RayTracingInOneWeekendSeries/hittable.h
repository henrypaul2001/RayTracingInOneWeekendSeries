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

	virtual float pdf_value(const point3& origin, const vec3& direction) const {
		return 0.0f;
	}

	virtual vec3 random(const point3& origin) const {
		return vec3(1.0f, 0.0f, 0.0f);
	}
};

class translate : public hittable {
public:
	translate(shared_ptr<hittable> object, const vec3& offset) : object(object), offset(offset) {
		bbox = object->bounding_box() + offset;
	}

	bool hit(const ray& r, const interval ray_t, hit_record& out_hit) const override {
		// Move ray backwards by the offset
		ray offset_r(r.Origin() - offset, r.Direction(), r.time());

		// Determine whether an intersection exists along offset ray
		if (!object->hit(offset_r, ray_t, out_hit)) {
			return false;
		}

		// Move intersection point forwards by offset
		out_hit.p += offset;

		return true;
	}

	aabb bounding_box() const override { return bbox; }

private:
	shared_ptr<hittable> object;
	vec3 offset;
	aabb bbox;
};

class rotate_y : public hittable {
public:
	rotate_y(shared_ptr<hittable> object, const float angle) : object(object) {
		float radians = degrees_to_radians(angle);
		sin_theta = std::sin(radians);
		cos_theta = std::cos(radians);
		bbox = object->bounding_box();

		point3 min = point3(infinity);
		point3 max = point3(-infinity);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
					auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
					auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

					auto newx = cos_theta * x + sin_theta * z;
					auto newz = -sin_theta * x + cos_theta * z;

					vec3 tester(newx, y, newz);

					for (int c = 0; c < 3; c++) {
						min[c] = std::fmin(min[c], tester[c]);
						max[c] = std::fmax(max[c], tester[c]);
					}
				}
			}
		}

		bbox = aabb(min, max);
	}

	bool hit(const ray& r, const interval ray_t, hit_record& out_hit) const override {
		// Transform ray into object space
		vec3 origin = point3(
			(cos_theta * r.Origin().x()) - (sin_theta * r.Origin().z()),
			r.Origin().y(),
			(sin_theta * r.Origin().x()) + (cos_theta * r.Origin().z())
		);

		vec3 direction = vec3(
			(cos_theta * r.Direction().x()) - (sin_theta * r.Direction().z()),
			r.Direction().y(),
			(sin_theta * r.Direction().x()) + (cos_theta * r.Direction().z())
		);

		ray rotated_r = ray(origin, direction, r.time());

		// Determine whether an intersection exists in object space
		if (!object->hit(rotated_r, ray_t, out_hit)) {
			return false;
		}

		// Transform the intersection back to world space
		out_hit.p = point3(
			(cos_theta * out_hit.p.x()) + (sin_theta * out_hit.p.z()),
			out_hit.p.y(),
			(-sin_theta * out_hit.p.x()) + (cos_theta * out_hit.p.z())
		);

		out_hit.normal = vec3(
			(cos_theta * out_hit.normal.x()) + (sin_theta * out_hit.normal.z()),
			out_hit.normal.y(),
			(-sin_theta * out_hit.normal.x()) + (cos_theta * out_hit.normal.z())
		);

		return true;
	}

	aabb bounding_box() const override { return bbox; }

private:
	shared_ptr<hittable> object;
	float sin_theta;
	float cos_theta;
	aabb bbox;
};