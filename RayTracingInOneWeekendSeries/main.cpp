#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

int main()
{
    // Materials
    auto material_ground = make_shared<lambertian>(colour(0.8f, 0.8f, 0.0f));
    auto material_center = make_shared<lambertian>(colour(0.1f, 0.2f, 0.5f));
    auto material_left = make_shared<metal>(colour(0.8f), 0.3f);
    auto material_right = make_shared<metal>(colour(0.8f, 0.6f, 0.2f), 1.0f);

    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0.0f, -100.5f, -1.0f), 100.0f, material_ground));
    world.add(make_shared<sphere>(point3(0.0f, 0.0f, -1.2f), 0.5f, material_center));
    world.add(make_shared<sphere>(point3(-1.0f, 0.0f, -1.0f), 0.5f, material_left));
    world.add(make_shared<sphere>(point3(1.0f, 0.0f, -1.0f), 0.5f, material_right));

    // Camera
    camera cam;
    cam.aspect_ratio = 16.0f / 9.0f;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_bounces = 50;

    // Render
    cam.render(world);

    return 0;
}