#pragma once
class interval {
public:
	float min, max;

	interval();
	interval(const float min, const float max) : min(min), max(max) {}
	interval(const interval& a, const interval& b) {
		min = a.min <= b.min ? a.min : b.min;
		max = a.max >= b.max ? a.max : b.max;
	}

	float size() const { return max - min; }

	bool contains(const float x) const { return min <= x && x <= max; }

	bool surrounds(const float x) const { return min < x && x < max; }

	float clamp(const float x) const { 
		if (x < min) { return min; }
		if (x > max) { return max; }
		return x;
	}

	interval expand(const float delta) const {
		float padding = delta / 2.0f;
		return interval(min - padding, max + padding);
	}

	static const interval empty, universe;
};

inline interval operator+(const interval& ival, float displacement) {
	return interval(ival.min + displacement, ival.max + displacement);
}

inline interval operator+(float displacement, const interval& ival) {
	return ival + displacement;
}