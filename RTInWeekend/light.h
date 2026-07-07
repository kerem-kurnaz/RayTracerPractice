#ifndef LIGHT_H
#define LIGHT_H

#include "rtweekend.h"
#include "material.h"

class diffuse_light : public material {
  public:
    diffuse_light(const color& light_color) : emit_color(light_color) {}

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override {
        return false;
    }

    virtual color emitted() const override {
        return emit_color;
    }

  private:
    color emit_color;
};

#endif
