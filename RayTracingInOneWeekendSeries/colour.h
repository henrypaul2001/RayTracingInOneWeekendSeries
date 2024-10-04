#pragma once
#include "vec3.h"

using colour = vec3;
void WriteColour(std::ostream& out, const colour& rgbColour) {
	float r = rgbColour.r();
	float g = rgbColour.g();
	float b = rgbColour.b();

	// Translate from 0, 1 to 0, 255
	int rbyte = int(255.999 * r);
	int gbyte = int(255.999 * g);
	int bbyte = int(255.999 * b);

	// Write to stream
	out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}