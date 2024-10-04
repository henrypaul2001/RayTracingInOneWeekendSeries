#include <iostream>
#include "vec3.h"
#include "colour.h"
int main()
{
    const int image_width = 256, image_height = 256;

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            colour pixel = colour(float(i) / (image_width - 1), float(j) / (image_height - 1), 0.0f);
            WriteColour(std::cout, pixel);
        }
    }

    std::clog << "\rDone.           \n";

    return 0;
}