#pragma once

#include "colour.h"

class texture {
public:
	virtual ~texture() = default;
	virtual colour value(const float u, const float v, const point3& p) const = 0;
};

class solid_colour : public texture {
public:
	solid_colour(const colour& albedo) : albedo(albedo) {}
	solid_colour(const float red, const float green, const float blue) : solid_colour(colour(red, green, blue)) {}

	colour value(const float u, const float v, const point3& p) const override {
		return albedo;
	}

private:
	colour albedo;
};