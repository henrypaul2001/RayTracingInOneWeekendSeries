#pragma once

#include "pdf.h"

class sphere_pdf : public pdf {
public:
	sphere_pdf() {}

	float value(const vec3& direction) const override {
		return 1.0f / (4.0f * pi);
	}

	vec3 generate() const override {
		return random_unit_vector();
	}
};