#pragma once

#include "rtweekend.h"
#include "onb.h"

class pdf {
public:
	virtual ~pdf() {}

	virtual float value(const vec3& direction) const = 0;
	virtual vec3 generate() const = 0;
};