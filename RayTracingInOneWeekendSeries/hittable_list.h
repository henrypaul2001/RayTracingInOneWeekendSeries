#pragma once
#include "hittable.h"
#include <vector>

class hittable_list : public hittable {
public:
	std::vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) {
		objects.push_back(object);
		bbox = aabb(bbox, object->bounding_box());
	}

	bool hit(const ray& r, const interval ray_t, hit_record& out_hit) const override {
		hit_record temp_hit;
		bool hit_anything = false;
		float closest_so_far = ray_t.max;

		for (const shared_ptr<hittable>& object : objects) {
			if (object->hit(r, interval(ray_t.min, closest_so_far), temp_hit)) {
				hit_anything = true;
				closest_so_far = temp_hit.t;
				out_hit = temp_hit;
			}
		}

		return hit_anything;
	}

	aabb bounding_box() const override { return bbox; }

	float pdf_value(const point3& origin, const vec3& direction) const override {
		float weight = 1.0f / objects.size();
		float sum = 0.0f;

		for (const auto& object : objects) {
			sum += weight * object->pdf_value(origin, direction);
		}

		return sum;
	}

	vec3 random(const point3& origin) const override {
		int int_size = int(objects.size());
		return objects[random_int(0, int_size - 1)]->random(origin);
	}

private:
	aabb bbox;
};