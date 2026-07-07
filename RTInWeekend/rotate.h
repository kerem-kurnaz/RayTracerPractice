#ifndef ROTATE_H
#define ROTATE_H

#include "rtweekend.h"
#include "hittable.h"

class rotate_y : public hittable {
  public:
    rotate_y(shared_ptr<hittable> p, double angle) : object(p) {
        auto radians = degrees_to_radians(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        bbox = object->bounding_box();

        point3 min( infinity,  infinity,  infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
        origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

        direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
        direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

        ray rotated_r(origin, direction, r.time());

        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        auto p = rec.p;
        p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
        p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

        auto normal = rec.normal;
        normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
        normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

        rec.p = p;
        rec.normal = unit_vector(normal);

        return true;
    }

    aabb bounding_box() const override { return bbox; }

    color get_emission() const override { return object->get_emission(); }

  private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};

class rotate_x : public hittable {
  public:
    rotate_x(shared_ptr<hittable> p, double angle) : object(p) {
        auto radians = degrees_to_radians(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        bbox = object->bounding_box();

        point3 min( infinity,  infinity,  infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newy = cos_theta*y - sin_theta*z;
                    auto newz = sin_theta*y + cos_theta*z;

                    vec3 tester(x, newy, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[1] = cos_theta*r.origin()[1] + sin_theta*r.origin()[2];
        origin[2] = -sin_theta*r.origin()[1] + cos_theta*r.origin()[2];

        direction[1] = cos_theta*r.direction()[1] + sin_theta*r.direction()[2];
        direction[2] = -sin_theta*r.direction()[1] + cos_theta*r.direction()[2];

        ray rotated_r(origin, direction, r.time());

        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        auto p = rec.p;
        p[1] = cos_theta*rec.p[1] - sin_theta*rec.p[2];
        p[2] = sin_theta*rec.p[1] + cos_theta*rec.p[2];

        auto normal = rec.normal;
        normal[1] = cos_theta*rec.normal[1] - sin_theta*rec.normal[2];
        normal[2] = sin_theta*rec.normal[1] + cos_theta*rec.normal[2];

        rec.p = p;
        rec.normal = unit_vector(normal);

        return true;
    }

    aabb bounding_box() const override { return bbox; }

    color get_emission() const override { return object->get_emission(); }

  private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};

class rotate_z : public hittable {
  public:
    rotate_z(shared_ptr<hittable> p, double angle) : object(p) {
        auto radians = degrees_to_radians(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        bbox = object->bounding_box();

        point3 min( infinity,  infinity,  infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx = cos_theta*x - sin_theta*y;
                    auto newy = sin_theta*x + cos_theta*y;

                    vec3 tester(newx, newy, z);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta*r.origin()[0] + sin_theta*r.origin()[1];
        origin[1] = -sin_theta*r.origin()[0] + cos_theta*r.origin()[1];

        direction[0] = cos_theta*r.direction()[0] + sin_theta*r.direction()[1];
        direction[1] = -sin_theta*r.direction()[0] + cos_theta*r.direction()[1];

        ray rotated_r(origin, direction, r.time());

        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        auto p = rec.p;
        p[0] = cos_theta*rec.p[0] - sin_theta*rec.p[1];
        p[1] = sin_theta*rec.p[0] + cos_theta*rec.p[1];

        auto normal = rec.normal;
        normal[0] = cos_theta*rec.normal[0] - sin_theta*rec.normal[1];
        normal[1] = sin_theta*rec.normal[0] + cos_theta*rec.normal[1];

        rec.p = p;
        rec.normal = unit_vector(normal);

        return true;
    }

    aabb bounding_box() const override { return bbox; }

    color get_emission() const override { return object->get_emission(); }

  private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};

#endif
