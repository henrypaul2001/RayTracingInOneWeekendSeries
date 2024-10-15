#pragma once

#include "hittable_list.h"
#include "pdf.h"

class hittable_pdf : public pdf {
public:
	hittable_pdf(const hittable& objects, const point3& origin) : objects(objects), origin(origin) {}

	float value(const vec3& direction) const override {
		return objects.pdf_value(origin, direction);
	}

	vec3 generate() const override {
		return objects.random(origin);
	}

private:
	const hittable& objects;
	point3 origin;
};