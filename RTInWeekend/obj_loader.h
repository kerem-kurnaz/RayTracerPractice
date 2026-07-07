#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "rtweekend.h"
#include "triangle.h"
#include "hittable_list.h"

hittable_list load_obj(
    const std::string& filename, 
    shared_ptr<material> mat, 
    double scale = 1.0, 
    const point3& offset = point3(0,0,0),
    double rotate_x_deg = 0.0,
    double rotate_y_deg = 0.0,
    double rotate_z_deg = 0.0
) {
    hittable_list triangles;
    std::vector<point3> vertices;
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Could not open the file - '" << filename << "'" << std::endl;
        return hittable_list();
    }

    // Heuristic pre-allocation of vertex vector capacity based on file size
    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    if (file_size > 0) {
        vertices.reserve(file_size / 60);
    }

    // Precompute trigonometric matrices for rotations
    double rad_x = degrees_to_radians(rotate_x_deg);
    double sin_x = sin(rad_x), cos_x = cos(rad_x);

    double rad_y = degrees_to_radians(rotate_y_deg);
    double sin_y = sin(rad_y), cos_y = cos(rad_y);

    double rad_z = degrees_to_radians(rotate_z_deg);
    double sin_z = sin(rad_z), cos_z = cos(rad_z);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        const char* p = line.c_str();
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '#') continue;

        if (p[0] == 'v' && (p[1] == ' ' || p[1] == '\t')) {
            p += 2;
            char* next;
            double x = std::strtod(p, &next); p = next;
            double y = std::strtod(p, &next); p = next;
            double z = std::strtod(p, &next);

            if (rotate_x_deg != 0.0) {
                double y_rot = y * cos_x - z * sin_x;
                double z_rot = y * sin_x + z * cos_x;
                y = y_rot;
                z = z_rot;
            }
            if (rotate_y_deg != 0.0) {
                double x_rot = x * cos_y + z * sin_y;
                double z_rot = -x * sin_y + z * cos_y;
                x = x_rot;
                z = z_rot;
            }
            if (rotate_z_deg != 0.0) {
                double x_rot = x * cos_z - y * sin_z;
                double y_rot = x * sin_z + y * cos_z;
                x = x_rot;
                y = y_rot;
            }

            vertices.emplace_back(x * scale + offset.x(), y * scale + offset.y(), z * scale + offset.z());
        } else if (p[0] == 'f' && (p[1] == ' ' || p[1] == '\t')) {
            p += 2;
            std::vector<int> face_vertices;
            while (*p != '\0') {
                while (*p == ' ' || *p == '\t') p++;
                if (*p == '\0' || *p == '\r' || *p == '\n') break;

                char* next;
                long v_idx = std::strtol(p, &next, 10);
                p = next;
                while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;

                int idx;
                if (v_idx > 0) {
                    idx = static_cast<int>(v_idx - 1);
                } else if (v_idx < 0) {
                    idx = static_cast<int>(vertices.size()) + static_cast<int>(v_idx);
                } else {
                    continue; // 0 is invalid index in OBJ
                }
                face_vertices.push_back(idx);
            }

            if (face_vertices.size() >= 3) {
                for (size_t i = 1; i < face_vertices.size() - 1; ++i) {
                    triangles.add(make_shared<triangle>(
                        vertices[face_vertices[0]], 
                        vertices[face_vertices[i]], 
                        vertices[face_vertices[i + 1]], 
                        mat
                    ));
                }
            }
        }
    }

    file.close();
    return triangles;
}

#endif
