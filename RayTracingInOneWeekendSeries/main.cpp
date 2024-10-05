#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

colour ray_colour(const ray& r, const hittable& world) {

    hit_record rec;
    if (world.hit(r, 0.0f, infinity, rec)) {
        return 0.5f * (rec.normal + colour(1.0f));
    }

    vec3 unit_direction = normalize(r.Direction());
    float a = 0.5f * (unit_direction.y() + 1.0f); // -1, 1 to 0, 1
    return lerp(colour(1.0f), colour(0.5f, 0.7f, 1.0f), a);
}

int main()
{
    // Image
    const float aspect_ratio = 16.0f / 9.0f;
    const int image_width = 400;

    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0.0f, 0.0f, -1.0f), 0.5f));
    world.add(make_shared<sphere>(point3(0.0f, -100.5f, -1.0f), 100.0f));

    // Camera
    float focal_length = 1.0f;
    float viewport_height = 2.0f;
    float viewport_width = viewport_height * (float(image_width) / image_height);
    point3 camera_center = point3(0.0f);

    // Calculate the vectors across viewport edges
    vec3 viewport_u = vec3(viewport_width, 0.0f, 0.0f);
    vec3 viewport_v = vec3(0.0f, -viewport_height, 0.0f);

    // Pixel delta vectors
    vec3 pixel_delta_u = viewport_u / image_width;
    vec3 pixel_delta_v = viewport_v / image_height;

    // Upper left pixel location
    point3 viewport_upper_left = camera_center - vec3(0.0f, 0.0f, focal_length) - viewport_u / 2.0f - viewport_v / 2.0f;
    point3 pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            point3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            vec3 ray_direction = pixel_center - camera_center;
            ray r = ray(camera_center, ray_direction);

            colour pixel_colour = ray_colour(r, world);
            WriteColour(std::cout, pixel_colour);
        }
    }

    std::clog << "\rDone.           \n";

    return 0;
}