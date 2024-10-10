#pragma once
#include "quad.h"

class disk : public quad {
public:
	disk(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : quad(Q, u, v, mat) {}

private:
	bool is_interior(const float a, const float b, hit_record& rec) const override {
		if (std::sqrt(a * a + b * b) < 1.0f) {
			rec.u = a / 2.0f + 0.5;
			rec.v = b / 2.0f + 0.5;
			return true;
		}
		else {
			return false;
		}
	}
};