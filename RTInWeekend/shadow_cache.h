#ifndef SHADOW_CACHE_H
#define SHADOW_CACHE_H

#include "hittable.h"

struct shadow_cache {
    const hittable* blocker = nullptr;
};

inline bool hit_shadow(const hittable& world, const ray& r, interval ray_t, shadow_cache& cache) {
    if (cache.blocker != nullptr) {
        hit_record rec;
        if (cache.blocker->hit(r, ray_t, rec)) {
            return true;
        }
    }

    hit_record rec;
    if (world.hit(r, ray_t, rec)) {
        cache.blocker = rec.top_level_object ? rec.top_level_object : rec.object;
        return true;
    }

    return false;
}

#endif
