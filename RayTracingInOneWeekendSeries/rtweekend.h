#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

// C++ std usings
using std::make_shared;
using std::shared_ptr;

// Constant
const double infinity_d = std::numeric_limits<double>::infinity();
const float infinity = std::numeric_limits<float>::infinity();
const double pi = 3.1415926535897932385;

// Utility
inline float degrees_to_radians(const float degrees) {
	return degrees * pi / 180.0f;
}

// Common headers
#include "ray.h"
#include "vec3.h"
#include "colour.h"
#include "interval.h"