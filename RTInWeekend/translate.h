#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "rtweekend.h"
#include "hittable.h"

class translate : public hittable {
  public:
    translate(shared_ptr<hittable> p, const vec3& displacement)
      : object(p), offset(displacement)
    {
        bbox = object->bounding_box();
        point3 min_p(bbox.x.min, bbox.y.min, bbox.z.min);
        point3 max_p(bbox.x.max, bbox.y.max, bbox.z.max);
        bbox = aabb(min_p + offset, max_p + offset);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        ray offset_r(r.origin() - offset, r.direction(), r.time());
        if (!object->hit(offset_r, ray_t, rec))
            return false;
        rec.p += offset;
        return true;
    }

    aabb bounding_box() const override { return bbox; }

    color get_emission() const override { return object->get_emission(); }

  private:
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};

#endif
