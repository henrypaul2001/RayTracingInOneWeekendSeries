#pragma once
#include "hittable.h"
#include "hittable_list.h"

class quad : public hittable {
public:
	quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat) {
		vec3 n = cross(u, v);
		normal = normalize(n);
		D = dot(normal, Q);
		w = n / dot(n, n);
		
		area = n.length();

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

	float pdf_value(const point3& origin, const vec3& direction) const override {
		hit_record rec;
		if (!this->hit(ray(origin, direction), interval(0.001f, infinity), rec)) {
			return 0.0f;
		}

		float distance_squared = rec.t * rec.t * direction.length2();
		float cosine = std::fabs(dot(direction, rec.normal) / direction.length());

		return distance_squared / (cosine * area);
	}

	vec3 random(const point3& origin) const override {
		vec3 p = Q + (random_double() * u) + (random_double() * v);
		return p - origin;
	}

protected:
	point3 Q;
	vec3 u, v;
	vec3 w;
	shared_ptr<material> mat;
	aabb bbox;

	vec3 normal;
	float D;
	float area;

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

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat) {
	// Returns 3d box of 6 quads
	auto sides = make_shared<hittable_list>();

	// Construct two opposite vertices with min and max coordinates
	auto min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
	auto max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

	vec3 dx = vec3(max.x() - min.x(), 0.0f, 0.0f);
	vec3 dy = vec3(0.0f, max.y() - min.y(), 0.0f);
	vec3 dz = vec3(0.0f, 0.0f, max.z() - min.z());

	sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
	sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
	sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
	sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
	sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

	return sides;
}