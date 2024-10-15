#pragma once
#include "hittable.h"
#include "texture.h"
#include "sphere_pdf.h"
#include "cosine_pdf.h"

struct scatter_record {
	colour attenuation;
	shared_ptr<pdf> pdf_ptr;
	bool skip_pdf;
	ray skip_pdf_ray;
};

class material {
public:
	virtual ~material() = default;

	virtual colour emitted(const ray& r_in, const hit_record& rec, const float u, const float v, const point3& p) const {
		return colour(0.0f);
	}

	virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const {
		return false;
	}

	virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
		return 0.0f;
	}
};

class lambertian : public material {
public:
	lambertian(const colour& albedo) : tex(make_shared<solid_colour>(albedo)) {}
	lambertian(shared_ptr<texture> tex) : tex(tex) {}

	bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		srec.attenuation = tex->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
		srec.skip_pdf = false;
		return true;
	}

	float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
		float cos_theta = dot(rec.normal, normalize(scattered.Direction()));
		return cos_theta < 0.0f ? 0.0f : cos_theta / pi;
	}

private:
	shared_ptr<texture> tex;
};

class metal : public material {
public:
	metal(const colour& albedo, const float fuzz) : albedo(albedo), fuzz(fuzz < 1.0f ? fuzz : 1.0f) {}

	bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		vec3 reflected = reflect(r_in.Direction(), rec.normal);
		reflected = normalize(reflected) + (fuzz * fast_random_unit_vector());

		srec.attenuation = albedo;
		srec.pdf_ptr = nullptr;
		srec.skip_pdf = true;
		srec.skip_pdf_ray = ray(rec.p, reflected, r_in.time());

		return true;
	}

private:
	colour albedo;
	float fuzz;
};

class dielectric : public material {
public:
	dielectric(float refraction_index) : refraction_index(refraction_index) {}

	bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		srec.attenuation = colour(1.0f);
		srec.pdf_ptr = nullptr;
		srec.skip_pdf = true;
		float ri = rec.front_face ? (1.0f / refraction_index) : refraction_index;

		vec3 unit_direction = normalize(r_in.Direction());
		float cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0f);
		float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_theta > 1.0f;
		vec3 direction;

		if (cannot_refract || reflectance(cos_theta, ri) > random_double()) {
			direction = reflect(unit_direction, rec.normal);
		}
		else {
			direction = refract(unit_direction, rec.normal, ri);
		}

		srec.skip_pdf_ray = ray(rec.p, direction, r_in.time());
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

class diffuse_light : public material {
public:
	diffuse_light(shared_ptr<texture> tex) : tex(tex) {}
	diffuse_light(const colour& emit) : tex(make_shared<solid_colour>(emit)) {}

	colour emitted(const ray& r_in, const hit_record& rec, const float u, const float v, const point3& p) const override {
		if (!rec.front_face) {
			return colour(0.0f);
		}
		return tex->value(u, v, p);
	}

private:
	shared_ptr<texture> tex;
};

class isotropic : public material {
public:
	isotropic(const colour& albedo) : tex(make_shared<solid_colour>(albedo)) {}
	isotropic(shared_ptr<texture> tex) : tex(tex) {}

	bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		srec.attenuation = tex->value(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared<sphere_pdf>();
		srec.skip_pdf = false;
		return true;
	}

	float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
		return 1.0f / (4.0f * pi);
	}

private:
	shared_ptr<texture> tex;
};