#pragma once
#include "hittable.h"

class quad : public hittable {
public:
	quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat) {
		vec3 n = cross(u, v);
		normal = normalize(n);
		D = dot(normal, Q);
		w = n / dot(n, n);
		
		set_bounding_box();
	}

	virtual void set_bounding_box() {
		auto bbox_diagonal1 = aabb(Q, Q + u + v);
		auto bbox_diagonal2 = aabb(Q + u, Q + v);
		bbox = aabb(bbox_diagonal1, bbox_diagonal2);
	}

	aabb bounding_box() const override { return bbox; }

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		float denom = dot(normal, r.Direction());

		// No hit if ray is parallel to plane
		if (std::fabs(denom) < 1e-8) {
			return false;
		}

		// Interval check
		float t = (D - dot(normal, r.Origin())) / denom;
		if (!ray_t.contains(t)) {
			return false;
		}

		// Determine if the hit point lies within planar shape using plane coordinates
		point3 intersection = r.at(t);
		vec3 planar_hitpt_vector = intersection - Q;
		float alpha = dot(w, cross(planar_hitpt_vector, v));
		float beta = dot(w, cross(u, planar_hitpt_vector));

		if (!is_interior(alpha, beta, rec)) {
			return false;
		}

		rec.t = t;
		rec.p = intersection;
		rec.mat = mat;
		rec.set_face_normal(r, normal);

		return true;
	}

protected:
	point3 Q;
	vec3 u, v;
	vec3 w;
	shared_ptr<material> mat;
	aabb bbox;

	vec3 normal;
	float D;

	virtual bool is_interior(const float a, const float b, hit_record& rec) const {
		interval unit_interval = interval(0.0f, 1.0f);

		if (!unit_interval.contains(a) || !unit_interval.contains(b)) {
			return false;
		}

		rec.u = a;
		rec.v = b;
		return true;
	}
};