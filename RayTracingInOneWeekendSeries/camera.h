#pragma once
#include "hittable.h"
#include "material.h"

#include <algorithm>
#include <vector>
#include <execution>

class camera {
public:
	float aspect_ratio = 1.0f;	// Ratio of image width over height
	int image_width = 100;	    // Rendered image width in pixel count
	int samples_per_pixel = 10; // Number of random samples per pixel
	int max_bounces = 10;		// Maximum times a ray can bounce off of geometry
	colour background;			// Scene background colour

	float vfov = 90.0f;						   // Vertical field of view
	point3 lookfrom = point3(0.0f);			   // Camera position
	point3 lookat = point3(0.0f, 0.0f, -1.0f); // Point camera is looking at
	vec3 vup = vec3(0.0f, 1.0f, 0.0f);		   // Camera relative up direction

	float defocus_angle = 0.0f; // Variation angle of rays through each pixel
	float focus_dist = 10.0f;	// Distance from camera lookfrom point to plane of perfect focus

	void render(const hittable& world) {
		initialize();

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (int j = 0; j < image_height; j++) {
			std::clog << "\rScanlines remaining: " << (image_height - j) << "                             \r" << std::flush;
			for (int i = 0; i < image_width; i++) {
				colour pixel_colour = colour(0.0f);
				for (int s_j = 0; s_j < sqrt_spp; s_j++) {
					for (int s_i = 0; s_i < sqrt_spp; s_i++) {
						ray r = get_ray(i, j, s_i, s_j);
						pixel_colour += ray_colour(r, max_bounces, world);
					}
				}
				WriteColour(std::cout, pixel_samples_scale * pixel_colour);
			}
		}
		std::clog << "\rDone.					\n";
	}

	void render_threaded(const hittable& world) {
		initialize();

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		colour* colours = new colour[image_width * image_height];
#define MULTITHREADED 1
#define ULTRATHREADED 1
#define SUPERTHREADED 1

#if MULTITHREADED
		std::vector<int> verticalIter;
		verticalIter.reserve(image_height);
		for (int i = 0; i < image_height; i++) {
			verticalIter.push_back(i);
		}

		std::vector<int> horizontalIter;
		horizontalIter.reserve(image_width);
		for (int i = 0; i < image_width; i++) {
			horizontalIter.push_back(i);
		}

		std::vector<int> sampleIter;
		sampleIter.reserve(samples_per_pixel);
		for (int i = 0; i < samples_per_pixel; i++) {
			sampleIter.push_back(i);
		}

		std::atomic_int scanlinesRemainingAtomic = image_height;
		std::for_each(std::execution::par, verticalIter.begin(), verticalIter.end(), [this, &world, &colours, &scanlinesRemainingAtomic, &horizontalIter, &sampleIter](int j) {
#if ULTRATHREADED
			std::clog << "\rScanlines remaining: " << scanlinesRemainingAtomic << "                             \r" << std::flush;

			std::for_each(std::execution::par, horizontalIter.begin(), horizontalIter.end(), [this, &world, &colours, j, &sampleIter](int i) {
				colour pixel_colour = colour(0.0f);

#if SUPERTHREADED
				std::vector<colour> samples = std::vector<colour>(samples_per_pixel, colour(0.0f));

				std::for_each(std::execution::par, sampleIter.begin(), sampleIter.end(), [this, &world, &samples, i, j](int sample) {
					ray r = get_ray(i, j);
					samples[sample] = ray_colour(r, max_bounces, world);
				});

				for (const colour& sample : samples) {
					pixel_colour += sample;
				}
				colours[image_width * j + i] = pixel_samples_scale * pixel_colour;
#else
				for (int sample = 0; sample < samples_per_pixel; sample++) {
					ray r = get_ray(i, j);
					pixel_colour += ray_colour(r, max_bounces, world);
				}

				colours[image_width * j + i] = pixel_samples_scale * pixel_colour;
#endif
				}
			);
			scanlinesRemainingAtomic--;
#else
			std::clog << "\rScanlines remaining: " << scanlinesRemainingAtomic << "                             \r" << std::flush;
			for (int i = 0; i < image_width; i++) {
				colour pixel_colour = colour(0.0f);

				for (int sample = 0; sample < samples_per_pixel; sample++) {
					ray r = get_ray(i, j);
					pixel_colour += ray_colour(r, max_bounces, world);
				}

				colours[image_width * j + i] = pixel_samples_scale * pixel_colour;
			}
			scanlinesRemainingAtomic--;
#endif

		});
#else
		for (int j = 0; j < image_height; j++) {
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

			for (int i = 0; i < image_width; i++) {
				colour pixel_colour = colour(0.0f);
				for (int sample = 0; sample < samples_per_pixel; sample++) {
					ray r = get_ray(i, j);
					pixel_colour += ray_colour(r, max_bounces, world);
				}
				colours[image_width * j + i] = pixel_samples_scale * pixel_colour;
			}
		}
#endif

		for (int i = 0; i < image_width * image_height; i++) {
			WriteColour(std::cout, colours[i]);
		}

		delete[] colours;

		std::clog << "\rDone.					\n";
	}

private:
	int image_height;			// Rendered image height
	float pixel_samples_scale;  // Colour scale factor for sum of pixel samples
	int sqrt_spp;				// Square root of number of samples per pixel
	float recip_sqrt_spp;		// 1 / sqrt_spp
	point3 center;				// Camera center
	point3 pixel00_loc;			// Location of pixel 0, 0
	vec3 pixel_delta_u;			// Offset to pixel to the right
	vec3 pixel_delta_v;			// Offset to pixel below
	vec3 u, v, w;				// Camera frame basis vectors
	vec3 defocus_disk_u;		// Defocus disk horizontal radius
	vec3 defocus_disk_v;		// Defocus disk vertical radius

	void initialize() {
		image_height = int(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		pixel_samples_scale = 1.0f / samples_per_pixel;

		sqrt_spp = int(std::sqrt(samples_per_pixel));
		pixel_samples_scale = 1.0f / (sqrt_spp * sqrt_spp);
		recip_sqrt_spp = 1.0f / sqrt_spp;

		center = lookfrom;

		// Determine viewport dimensions
		float theta = degrees_to_radians(vfov);
		float h = std::tan(theta / 2.0f);
		auto viewport_height = 2.0 * h * focus_dist;
		auto viewport_width = viewport_height * (double(image_width) / image_height);

		// Calculate u, v, w unit basis vectors for camera coordinate frame
		w = normalize(lookfrom - lookat);
		u = normalize(cross(vup, w));
		v = cross(w, u);

		// Calculate the vectors across viewport edges
		auto viewport_u = viewport_width * u;
		auto viewport_v = viewport_height * -v;

		// Pixel delta vectors
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Upper left pixel location
		auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.0f - viewport_v / 2.0f;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

		// Calculate camera defocus disk basis vectors
		float defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2.0f));
		defocus_disk_u = u * defocus_radius;
		defocus_disk_v = v * defocus_radius;
	}

	ray get_ray(int i, int j) const {
		// Construct ray originating from defocus disk and directed and randomly sampled point around pixel location i, j
		vec3 offset = sample_square();
		vec3 pixel_sample = pixel00_loc
			+ ((i + offset.x()) * pixel_delta_u)
			+ ((j + offset.y()) * pixel_delta_v);

		point3 ray_origin = (defocus_angle <= 0.0f) ? center : defocus_disk_sample();
		vec3 ray_direction = pixel_sample - ray_origin;
		double ray_time = fast_random_double();

		return ray(ray_origin, ray_direction, ray_time);
	}

	ray get_ray(int i, int j, int s_i, int s_j) const {
		// Construct ray originating from defocus disk and directed and randomly sampled point around pixel location i, j for stratified sample square s_i, s_j
		vec3 offset = sample_square_stratified(s_i, s_j);
		vec3 pixel_sample = pixel00_loc
			+ ((i + offset.x()) * pixel_delta_u)
			+ ((j + offset.y()) * pixel_delta_v);

		point3 ray_origin = (defocus_angle <= 0.0f) ? center : defocus_disk_sample();
		vec3 ray_direction = pixel_sample - ray_origin;
		double ray_time = fast_random_double();

		return ray(ray_origin, ray_direction, ray_time);
	}

	vec3 sample_square() const {
		// Returns vector to a random point in the -.5, -.5 to .5, .5 square
		return vec3(fast_random_double() - 0.5f, fast_random_double() - 0.5f, 0.0f);
	}

	vec3 sample_square_stratified(int s_i, int s_j) const {
		// Returns vector to random point in square sub-pixel specified by grid indices s_i, s_j
		auto px = ((s_i + fast_random_double()) * recip_sqrt_spp) - 0.5;
		auto py = ((s_j + fast_random_double()) * recip_sqrt_spp) - 0.5;

		return vec3(px, py, 0.0f);
	}

	point3 defocus_disk_sample() const {
		// Returns random point in camera focus disk
		point3 p = fast_random_in_unit_disk();
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	colour ray_colour(const ray& r, const int depth, const hittable& world) const {
		if (depth <= 0) {
			return colour(0.0f);
		}

		hit_record rec;

		if (!world.hit(r, interval(0.001f, infinity), rec)) {
			return background;
		}

		ray scattered;
		colour attenuation;
		float pdf_value;
		colour colour_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

		if (!rec.mat->scatter(r, rec, attenuation, scattered, pdf_value)) {
			return colour_from_emission;
		}

		float scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);
		pdf_value = scattering_pdf;

		colour colour_from_scatter = (attenuation * scattering_pdf * ray_colour(scattered, depth - 1, world)) / pdf_value;

		return colour_from_emission + colour_from_scatter;
	}
};