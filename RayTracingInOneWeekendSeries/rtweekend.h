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
static uint32_t xorshift32_state = 0x12345678; // Example seed

inline uint32_t xorshift32() {
	uint32_t x = xorshift32_state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return xorshift32_state = x;
}

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

inline double fast_random_double() {
	return (xorshift32() / (double)UINT32_MAX);
}

inline double fast_random_double(const double min, const double max) {
	return min + (max - min) * fast_random_double();
}

inline int fast_random_int(const int min, const int max) {
	//return int(fast_random_double(seed, min, max + 1));
	return min + (xorshift32() % (max - min + 1));
}

// Common headers
#include "ray.h"
#include "vec3.h"
#include "colour.h"
#include "interval.h"