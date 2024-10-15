#pragma once

#include "pdf.h"

class cosine_pdf : public pdf {
public:
	cosine_pdf(const vec3& w) : uvw(w) {}

	float value(const vec3& direction) const override {
		float cosine_theta = dot(normalize(direction), uvw.w());
		return std::fmax(0.0f, cosine_theta / pi);
	}

	vec3 generate() const override {
		return uvw.transform(random_cosine_direction());
	}

private:
	onb uvw;
};