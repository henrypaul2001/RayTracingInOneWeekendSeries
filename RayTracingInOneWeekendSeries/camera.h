#pragma once
#include "hittable.h"

class camera {
public:
	float aspect_ratio = 1.0f; // Ratio of image width over height
	int image_width = 100;	   // Rendered image width in pixel count

	void render(const hittable& world) {
		initialize();

		std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
		for (int j = 0; j < image_height; j++) {
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
			for (int i = 0; i < image_width; i++) {
				point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
				vec3 ray_direction = pixel_center - center;
				ray r = ray(center, ray_direction);

				colour pixel_colour = ray_colour(r, world);
				WriteColour(std::cout, pixel_colour);
			}
		}

		std::clog << "\rDone.           \n";
	}

private:
	int image_height;    // Rendered image height
	point3 center;       // Camera center
	point3 pixel00_loc;  // Location of pixel 0, 0
	vec3 pixel_delta_u;  // Offset to pixel to the right
	vec3 pixel_delta_v;  // Offset to pixel below

	void initialize() {
		image_height = int(image_width / aspect_ratio);
		image_height = (image_height < 1) ? 1 : image_height;

		center = point3(0, 0, 0);

		// Determine viewport dimensions
		auto focal_length = 1.0;
		auto viewport_height = 2.0;
		auto viewport_width = viewport_height * (double(image_width) / image_height);

		// Calculate the vectors across viewport edges
		auto viewport_u = vec3(viewport_width, 0, 0);
		auto viewport_v = vec3(0, -viewport_height, 0);

		// Pixel delta vectors
		pixel_delta_u = viewport_u / image_width;
		pixel_delta_v = viewport_v / image_height;

		// Upper left pixel location
		auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
		pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
	}

	colour ray_colour(const ray& r, const hittable& world) const {
		hit_record rec;
		if (world.hit(r, interval(0.0f, infinity), rec)) {
			return 0.5f * (rec.normal + colour(1.0f));
		}

		vec3 unit_direction = normalize(r.Direction());
		float a = 0.5f * (unit_direction.y() + 1.0f); // -1, 1 to 0, 1
		return lerp(colour(1.0f), colour(0.5f, 0.7f, 1.0f), a);
	}
};