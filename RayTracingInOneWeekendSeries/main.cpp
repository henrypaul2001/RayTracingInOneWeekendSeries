#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

#include <chrono>
#include "bvh_node.h"

void bouncing_spheres(hittable_list& world, camera& cam) {
    auto ground_material = make_shared<lambertian>(colour(0.5f));
    world.add(make_shared<sphere>(point3(0, -1000.0f, 0), 1000.0f, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center = point3(a + 0.9f * random_double(), 0.2f, b + 0.9f * random_double());

            if ((center - point3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    colour albedo = colour::random() * colour::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    point3 center2 = center + vec3(0.0f, random_double(0.0, 0.5), 0.0f);
                    world.add(make_shared<sphere>(center, center2, 0.2f, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    colour albedo = colour::random(0.5f, 1.0f);
                    float fuzz = random_double(0.0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2f, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5f);
                    world.add(make_shared<sphere>(center, 0.2f, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5f);
    world.add(make_shared<sphere>(point3(0.0f, 1.0f, 0.0f), 1.0f, material1));

    auto material2 = make_shared<lambertian>(colour(0.4f, 0.2f, 0.1f));
    world.add(make_shared<sphere>(point3(-4.0f, 1.0f, 0.0f), 1.0f, material2));

    auto material3 = make_shared<metal>(colour(0.7f, 0.6f, 0.5f), 0.0f);
    world.add(make_shared<sphere>(point3(4.0f, 1.0f, 0.0f), 1.0f, material3));

    world = hittable_list(make_shared<bvh_node>(world));

    cam.aspect_ratio = 16.0f / 9.0f;
    cam.image_width = 1080;
    cam.samples_per_pixel = 50;
    cam.max_bounces = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13.0f, 2.0f, 3.0f);
    cam.lookat = point3(0.0f);
    cam.vup = vec3(0.0f, 1.0f, 0.0f);

    cam.defocus_angle = 0.8f;
    cam.focus_dist = 10.0f;
}

void SphereLine(hittable_list& world, camera& cam) {
    // Materials
    auto material_ground = make_shared<lambertian>(colour(0.8f, 0.8f, 0.0f));
    auto material_center = make_shared<lambertian>(colour(0.1f, 0.2f, 0.5f));
    auto material_left = make_shared<dielectric>(1.5f);
    auto material_bubble = make_shared<dielectric>(1.0f / 1.5f);
    auto material_right = make_shared<metal>(colour(0.8f, 0.6f, 0.2f), 1.0f);

    // World
    world.add(make_shared<sphere>(point3(0.0f, -100.5f, -1.0f), 100.0f, material_ground));
    world.add(make_shared<sphere>(point3(0.0f, 0.0f, -1.2f), 0.5f, material_center));
    world.add(make_shared<sphere>(point3(-1.0f, 0.0f, -1.0f), 0.5f, material_left));
    world.add(make_shared<sphere>(point3(-1.0f, 0.0f, -1.0f), 0.4f, material_bubble));
    world.add(make_shared<sphere>(point3(1.0f, 0.0f, -1.0f), 0.5f, material_right));

    // Camera
    cam.aspect_ratio = 16.0f / 9.0f;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_bounces = 50;

    cam.vfov = 20.0f;
    cam.lookfrom = point3(-2.0f, 2.0f, 1.0f);
    cam.lookat = point3(0.0f, 0.0f, -1.0f);
    cam.vup = vec3(0.0f, 1.0f, 0.0f);

    cam.defocus_angle = 10.0f;
    cam.focus_dist = 3.4f;
}

void CheckeredSpheres(hittable_list& world, camera& cam) {
    auto checker = make_shared<checker_texture>(0.32f, colour(0.2f, 0.3f, 0.1f), colour(0.9f));

    world.add(make_shared<sphere>(point3(0.0f, -10.0f, 0.0f), 10.0f, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0.0f, 10.0f, 0.0f), 10.0f, make_shared<lambertian>(checker)));

    cam.aspect_ratio = 16.0f / 9.0f;
    cam.image_width = 500;
    cam.samples_per_pixel = 100;
    cam.max_bounces = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13.0f, 2.0f, 3.0f);
    cam.lookat = point3(0.0f);
    cam.vup = vec3(0.0f, 1.0f, 0.0f);

    cam.defocus_angle = 0.0f;
}

void Earth(hittable_list& world, camera& cam) {
    auto earth_texture = make_shared<image_texture>("images/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0.0f), 2.0f, earth_surface);

    world.add(globe);

    cam.aspect_ratio = 16.0f / 9.0f;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_bounces = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(0.0f, 4.0f, 12.0f);
    cam.lookat = point3(0.0f);
    cam.vup = vec3(0.0f, 1.0f, 0.0f);

    cam.defocus_angle = 0;
}

void PerlinSpheres(hittable_list& world, camera& cam) {
    auto pertext = make_shared<noise_texture>();
    world.add(make_shared<sphere>(point3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0.0f, 2.0f, 0.0f), 2.0f, make_shared<lambertian>(pertext)));

    cam.aspect_ratio = 16.0f / 9.0f;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_bounces = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13.0f, 2.0f, 3.0f);
    cam.lookat = point3(0.0f);
    cam.vup = vec3(0.0f, 1.0f, 0.0f);

    cam.defocus_angle = 0;
}

int main()
{
    camera cam;
    hittable_list world;
    
    PerlinSpheres(world, cam);

    cam.image_width = 1920;

    auto start = std::chrono::high_resolution_clock::now();

    // Render
    cam.render(world);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    // Extract hours, minutes, and seconds from the total duration
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = duration; // Remaining seconds

    // Display the time elapsed
    std::clog << "Time elapsed: "
        << hours.count() << " hours, "
        << minutes.count() << " minutes, "
        << seconds.count() << " seconds";

    char c;
    std::cin >> c;

    return 0;
}