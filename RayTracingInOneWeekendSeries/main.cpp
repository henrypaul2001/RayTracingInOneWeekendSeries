#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

int main()
{
    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0.0f, 0.0f, -1.0f), 0.5f));
    world.add(make_shared<sphere>(point3(0.0f, -100.5f, -1.0f), 100.0f));

    // Camera
    camera cam;
    cam.aspect_ratio = 16.0f / 9.0f;
    cam.image_width = 400;

    // Render
    cam.render(world);

    return 0;
}