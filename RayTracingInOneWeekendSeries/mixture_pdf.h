#pragma once
#include "pdf.h"

class mixture_pdf : public pdf {
public:
	mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1) {
		p[0] = p0;
		p[1] = p1;
	}

	float value(const vec3& direction) const override {
		return 0.5f * p[0]->value(direction) + 0.5f * p[1]->value(direction);
	}

	vec3 generate() const override {
		if (random_double() < 0.5) { return p[0]->generate(); }
		else { return p[1]->generate(); }
	}

private:
	shared_ptr<pdf> p[2];
};