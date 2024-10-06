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

class dielectric : public material {
public:
	dielectric(float refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
		attenuation = colour(1.0f);
		float ri = rec.front_face ? (1.0f / refraction_index) : refraction_index;

		vec3 unit_direction = normalize(r_in.Direction());

		float cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0f);
		float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_theta > 1.0f;

		vec3 direction;
		if (cannot_refract || reflectance(cos_theta, ri) > random_double()) { 
			direction = reflect(unit_direction, rec.normal); 
		}
		else { direction = refract(unit_direction, rec.normal, ri); }

		scattered = ray(rec.p, direction);
		return true;
	}

private:
	float refraction_index;

	static float reflectance(float cosine, float refraction_index) {
		// Schlick's reflectance approximation
		float r0 = (1.0f - refraction_index) / (1.0f + refraction_index);
		r0 = r0 * r0;
		return r0 + (1.0f - r0) * std::pow((1 - cosine), 5.0f);
	}
};