#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "rtweekend.h"

#include "hittable.h"

class triangle : public hittable {
public:
    triangle(point3 v0, point3 v1, point3 v2, shared_ptr<material> m)
        : vert0(v0), vert1(v1), vert2(v2), mat(m)
    {
        auto v0v1 = v1 - v0;
        auto v0v2 = v2 - v0;
        normal = unit_vector(cross(v0v1, v0v2));
        bbox = aabb(v0, v1).expand(v2);
    }

    void set_vertices(point3 v0, point3 v1, point3 v2) {
        vert0 = v0;
        vert1 = v1;
        vert2 = v2;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // Möller-Trumbore intersection algorithm
        auto v0v1 = vert1 - vert0;
        auto v0v2 = vert2 - vert0;
        auto pvec = cross(r.direction(), v0v2);
        auto det = dot(v0v1, pvec);

        // if the determinant is negative the triangle is backfacing
        // if the determinant is close to 0, the ray misses the triangle
        if (std::fabs(det) < 1e-8)
            return false;

        auto inv_det = 1.0 / det;

        auto tvec = r.origin() - vert0;
        auto u = dot(tvec, pvec) * inv_det;
        if (u < 0 || u > 1)
            return false;

        auto qvec = cross(tvec, v0v1);
        auto v = dot(r.direction(), qvec) * inv_det;
        if (v < 0 || u + v > 1)
            return false;

        auto t = dot(v0v2, qvec) * inv_det;
        if (!ray_t.contains(t))
            return false;

        rec.t = t;
        rec.p = r.at(t);
        rec.set_face_normal(r, normal);
        rec.mat = mat;
        rec.object = this;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

    color get_emission() const override { return mat->emitted(); }

private:
    point3 vert0, vert1, vert2;
    shared_ptr<material> mat;
    vec3 normal;
    aabb bbox;
};

#endif
