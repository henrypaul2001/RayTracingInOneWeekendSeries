#pragma once
#include "hittable.h"
#include <vector>

using std::make_shared;
using std::shared_ptr;

class hittable_list : public hittable {
public:
	std::vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) {
		objects.push_back(object);
	}

	bool hit(const ray& r, const float ray_tmin, const float ray_tmax, hit_record& out_hit) const override {
		hit_record temp_hit;
		bool hit_anything = false;
		float closest_so_far = ray_tmax;

		for (const shared_ptr<hittable>& object : objects) {
			if (object->hit(r, ray_tmin, closest_so_far, temp_hit)) {
				hit_anything = true;
				closest_so_far = temp_hit.t;
				out_hit = temp_hit;
			}
		}

		return hit_anything;
	}
};