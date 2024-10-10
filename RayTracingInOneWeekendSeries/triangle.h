#pragma once
#include "quad.h"

class triangle : public quad {
public:
	triangle(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : quad(Q, u, v, mat) {}

private:
	bool is_interior(const float a, const float b, hit_record& rec) const override {
		if (a > 0.0f && b > 0.0f && a + b < 1.0f) {
			rec.u = a;
			rec.v = b;
			return true;
		}
		else {
			return false;
		}
	}
};