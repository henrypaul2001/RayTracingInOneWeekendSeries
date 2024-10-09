#pragma once
#include "rtweekend.h"

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

class checker_texture : public texture {
public:
	checker_texture(const float scale, shared_ptr<texture> even, shared_ptr<texture> odd) : inv_scale(1.0f / scale), even(even), odd(odd) {}
	checker_texture(const float scale, const colour& c1, const colour& c2) : checker_texture(scale, make_shared<solid_colour>(c1), make_shared<solid_colour>(c2)) {}

	colour value(const float u, const float v, const point3& p) const override {
		int xInteger = int(std::floor(inv_scale * p.x()));
		int yInteger = int(std::floor(inv_scale * p.y()));
		int zInteger = int(std::floor(inv_scale * p.z()));

		bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;
		return isEven ? even->value(u, v, p) : odd->value(u, v, p);
	}

private:
	float inv_scale;
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
};