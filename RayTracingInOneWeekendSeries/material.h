#pragma once
#include "hittable.h"

class material {
public:
	virtual ~material() = default;

	virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const {
		return false;
	}
};

class lambertian : public material {
public:
	lambertian(const colour& albedo) : albedo(albedo) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		vec3 scatter_direction = rec.normal + random_unit_vector();

		if (scatter_direction.near_zero()) {
			scatter_direction = rec.normal;
		}

		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}

private:
	colour albedo;
};

class metal : public material {
public:
	metal(const colour& albedo, const float fuzz) : albedo(albedo), fuzz(fuzz < 1.0f ? fuzz : 1.0f) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		vec3 reflected = reflect(r_in.Direction(), rec.normal);
		reflected = normalize(reflected) + (fuzz * random_unit_vector());
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return (dot(scattered.Direction(), rec.normal) > 0.0f);
	}

private:
	colour albedo;
	float fuzz;
};