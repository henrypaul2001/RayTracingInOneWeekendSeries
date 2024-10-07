#pragma once
#include "hittable.h"
#include "material.h"

class camera {
public:
	float aspect_ratio = 1.0f;	// Ratio of image width over height
	int image_width = 100;	    // Rendered image width in pixel count
	int samples_per_pixel = 10; // Number of random samples per pixel
	int max_bounces = 10;		// Maximum times a ray can bounce off of geometry

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
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			for (int i = 0; i < image_width; i++) {
				colour pixel_colour = colour(0.0f);
				for (int sample = 0; sample < samples_per_pixel; sample++) {
					ray r = get_ray(i, j);
					pixel_colour += ray_colour(r, max_bounces, world);
				}
				WriteColour(std::cout, pixel_samples_scale * pixel_colour);
			}
		}

		std::clog << "\rDone.					\n";
	}

private:
	int image_height;			// Rendered image height
	float pixel_samples_scale;  // Colour scale factor for sum of pixel samples
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

		return ray(ray_origin, ray_direction);
	}

	vec3 sample_square() const {
		// Returns vector to a random point in the -.5, -.5 to .5, .5 square
		return vec3(random_double() - 0.5f, random_double() - 0.5f, 0.0f);
	}

	point3 defocus_disk_sample() const {
		// Returns random point in camera focus disk
		point3 p = random_in_unit_disk();
		return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
	}

	colour ray_colour(const ray& r, const int depth, const hittable& world) const {
		if (depth <= 0) {
			return colour(0.0f);
		}

		hit_record rec;
		if (world.hit(r, interval(0.001f, infinity), rec)) {
			ray scattered;
			colour attenuation;
			if (rec.mat->scatter(r, rec, attenuation, scattered)) {
				return attenuation * ray_colour(scattered, depth - 1, world);
			}
			else {
				return colour(0.0f);
			}
		}

		vec3 unit_direction = normalize(r.Direction());
		float a = 0.5f * (unit_direction.y() + 1.0f); // -1, 1 to 0, 1
		return lerp(colour(1.0f), colour(0.5f, 0.7f, 1.0f), a);
	}
};