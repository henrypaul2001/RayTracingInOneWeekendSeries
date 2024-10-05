#pragma once
#include "vec3.h"
#include "interval.h"
using colour = vec3;
inline void WriteColour(std::ostream& out, const colour& rgbColour) {
	float r = rgbColour.r();
	float g = rgbColour.g();
	float b = rgbColour.b();

	// Translate from 0, 1 to 0, 255
	static const interval intensity = interval(0.000f, 0.999f);
	int rbyte = int(255.999 * intensity.clamp(r));
	int gbyte = int(255.999 * intensity.clamp(g));
	int bbyte = int(255.999 * intensity.clamp(b));

	// Write to stream
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}