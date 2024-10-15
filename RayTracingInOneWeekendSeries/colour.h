#pragma once
#include "vec3.h"
#include "interval.h"
using colour = vec3;

inline float linear_to_gamma(float linear_component) { if (linear_component > 0.0f) { return std::sqrt(linear_component); } else { return 0.0f; } }

inline void WriteColour(std::ostream& out, const colour& rgbColour) {
	float r = rgbColour.r();
	float g = rgbColour.g();
	float b = rgbColour.b();

	// NaN check
	if (r != r) { r = 0.0f; }
	if (g != g) { g = 0.0f; }
	if (b != b) { b = 0.0f; }

	// Gamma correction
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	// Translate from 0, 1 to 0, 255
	static const interval intensity = interval(0.000f, 0.999f);
	int rbyte = int(255.999 * intensity.clamp(r));
	int gbyte = int(255.999 * intensity.clamp(g));
	int bbyte = int(255.999 * intensity.clamp(b));

	// Write to stream
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}