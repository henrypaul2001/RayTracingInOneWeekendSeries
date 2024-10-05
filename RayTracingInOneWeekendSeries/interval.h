#pragma once
class interval {
public:
	float min, max;

	interval();
	interval(const float min, const float max) : min(min), max(max) {}

	float size() const { return max - min; }
	bool contains(const float x) const { return min <= x && x <= max; }
	bool surrounds(const float x) const { return min < x && x < max; }
	float clamp(const float x) const { 
		if (x < min) { return min; }
		if (x > max) { return max; }
		return x;
	}
	static const interval empty, universe;
};