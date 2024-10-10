#pragma once
#include "rtweekend.h"
#include "rtw_image.h"
#include "perlin.h"

class texture {
public:
	virtual ~texture() = default;
	virtual colour value(float u, float v, const point3& p) const = 0;
};

class solid_colour : public texture {
public:
	solid_colour(const colour& albedo) : albedo(albedo) {}
	solid_colour(const float red, const float green, const float blue) : solid_colour(colour(red, green, blue)) {}

	colour value(float u, float v, const point3& p) const override {
		return albedo;
	}

private:
	colour albedo;
};

class checker_texture : public texture {
public:
	checker_texture(const float scale, shared_ptr<texture> even, shared_ptr<texture> odd) : inv_scale(1.0f / scale), even(even), odd(odd) {}
	checker_texture(const float scale, const colour& c1, const colour& c2) : checker_texture(scale, make_shared<solid_colour>(c1), make_shared<solid_colour>(c2)) {}

	colour value(float u, float v, const point3& p) const override {
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

class image_texture : public texture {
public:
	image_texture(const char* filename) : image(filename) {}

	colour value(float u, float v, const point3& p) const override {
		// If no texture data, return solid cyan as debugging aid
		if (image.height() <= 0) { return colour(0.0f, 1.0f, 1.0f); }

		// Clamp input coordinates in 0, 1 range
		u = interval(0, 1).clamp(u);
		v = 1.0 - interval(0, 1).clamp(v); // Flip v axis

		int i = int(u * image.width());
		int j = int(v * image.height());
		auto pixel = image.pixel_data(i, j);

		float colour_scale = 1.0f / 255.0f;
		return colour(colour_scale * pixel[0], colour_scale * pixel[1], colour_scale * pixel[2]);
	}

private:
	rtw_image image;
};

class noise_texture : public texture {
public:
	noise_texture(const float scale) : scale(scale) {}

	colour value(float u, float v, const point3& p) const override {
		return colour(1.0f) * noise.noise(scale * p);
	}

private:
	perlin noise;
	float scale;
};