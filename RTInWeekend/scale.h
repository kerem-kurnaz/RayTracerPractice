#ifndef SCALE_H
#define SCALE_H

#include "rtweekend.h"
#include "hittable.h"

class scale : public hittable {
  public:
    scale(shared_ptr<hittable> p, double scale_factor)
      : object(p), inv_scale(1.0 / scale_factor), scale_val(scale_factor)
    {
        bbox = object->bounding_box();
        point3 min_p(bbox.x.min, bbox.y.min, bbox.z.min);
        point3 max_p(bbox.x.max, bbox.y.max, bbox.z.max);
        bbox = aabb(min_p * scale_val, max_p * scale_val);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto origin = r.origin() * inv_scale;
        auto direction = r.direction() * inv_scale;
        ray scaled_r(origin, direction, r.time());

        if (!object->hit(scaled_r, ray_t, rec))
            return false;

        rec.p = rec.p * scale_val;
        rec.normal = unit_vector(rec.normal * inv_scale);
        return true;
    }

    aabb bounding_box() const override { return bbox; }

    color get_emission() const override { return object->get_emission(); }

  private:
    shared_ptr<hittable> object;
    double inv_scale;
    double scale_val;
    aabb bbox;
};

#endif
