#pragma once

#include <cmath>
#include <cstdlib>
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

inline double random_double() {
	// Returns a random real 0 to 1
	return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(const double min, const double max) {
	// Returns a random real min to max
	return min + (max - min) * random_double();
}

inline int random_int(const int min, const int max) {
	// Returns a random integer min to max
	return int(random_double(min, max + 1));
}

// Common headers
#include "ray.h"
#include "vec3.h"
#include "colour.h"
#include "interval.h"