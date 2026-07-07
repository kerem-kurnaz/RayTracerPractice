#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"
#include "shadow_cache.h"
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <fstream>

class camera {
public:
  double aspect_ratio = 1.0;  // Ratio of image width over height
  int image_width = 100;      // Rendered image width in pixel count
  int samples_per_pixel = 10; // Count of random samples for each pixel
  int max_depth = 10;         // Maximum number of ray bounces into scene

  double vfov = 90;                  // Vertical view angle (field of view)
  point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
  point3 lookat = point3(0, 0, -1);  // Point camera is looking at
  vec3 vup = vec3(0, 1, 0);          // Camera-relative "up" direction

  double defocus_angle = 0; // Variation angle of rays through each pixel
  double focus_dist =
      10; // Distance from camera lookfrom point to plane of perfect focus

  shared_ptr<hittable_list> lights;
  color background = color(-1.0, -1.0, -1.0); // Default to -1 to trigger sky gradient

  void render(const hittable &world, const std::string& out_filename = "") {
    initialize();

    std::ofstream file_out;
    std::ostream* out = &std::cout;
    if (!out_filename.empty()) {
      file_out.open(out_filename, std::ios::binary);
      if (file_out.is_open()) {
        out = &file_out;
      } else {
        std::clog << "Failed to open output file: " << out_filename << ", rendering to stdout instead.\n";
      }
    }

    *out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    std::vector<color> image_buffer(image_width * image_height);
    std::atomic<int> next_row(0);
    std::atomic<int> rows_done(0);
    std::mutex clog_mutex;

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;

    std::vector<std::thread> threads;
    for (unsigned int t = 0; t < num_threads; ++t) {
      threads.emplace_back([&]() {
        while (true) {
          int j = next_row.fetch_add(1);
          if (j >= image_height)
            break;

          for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            double s1 = 0;
            double s2 = 0;
            int n = 0;

            const int min_samples = 8;
            const double tolerance = 0.03;

            for (int sample = 0; sample < samples_per_pixel; ++sample) {
              ray r = get_ray(i, j);
              color c = ray_color(r, max_depth, world);
              pixel_color += c;

              double y = 0.299 * c.x() + 0.587 * c.y() + 0.114 * c.z();
              s1 += y;
              s2 += y * y;
              n++;

              if (n >= min_samples && (n % 4 == 0)) {
                double variance = (s2 - (s1 * s1) / n) / (n - 1);
                if (variance < 0) variance = 0;
                double stdev = std::sqrt(variance);
                double confidence_interval = 1.96 * stdev / std::sqrt(n);
                double mean = s1 / n;

                if (confidence_interval < tolerance * mean) {
                  break;
                }
              }
            }
            image_buffer[j * image_width + i] = pixel_color / n;
          }

          int completed = rows_done.fetch_add(1);
          {
            std::lock_guard<std::mutex> lock(clog_mutex);
            std::clog << "\rScanlines remaining: " << (image_height - completed - 1) << ' ' << std::flush;
          }
        }
      });
    }

    for (auto &thread : threads) {
      thread.join();
    }

    for (int j = 0; j < image_height; ++j) {
      for (int i = 0; i < image_width; ++i) {
        write_color(*out, image_buffer[j * image_width + i]);
      }
    }

    std::clog << "\rDone.                 \n";
  }

private:
  int image_height;           // Rendered image height
  double pixel_samples_scale; // Color scale factor for a sum of pixel samples
  point3 center;              // Camera center
  point3 pixel00_loc;         // Location of pixel 0, 0
  vec3 pixel_delta_u;         // Offset to pixel to the right
  vec3 pixel_delta_v;         // Offset to pixel below
  vec3 u, v, w;               // Camera frame basis vectors
  vec3 defocus_disk_u;        // Defocus disk horizontal radius
  vec3 defocus_disk_v;        // Defocus disk vertical radius

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    pixel_samples_scale = 1.0 / samples_per_pixel;

    center = lookfrom;

    // Determine viewport dimensions.
    auto theta = degrees_to_radians(vfov);
    auto h = tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical
    // viewport edges.
    vec3 viewport_u =
        viewport_width * u; // Vector across viewport horizontal edge
    vec3 viewport_v =
        viewport_height * -v; // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left =
        center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    auto defocus_radius =
        focus_dist * tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  ray get_ray(int i, int j) const {
    // Construct a camera ray originating from the defocus disk and directed at
    // a randomly sampled point around the pixel location i, j.

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
                        ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = random_double();

    return ray(ray_origin, ray_direction, ray_time);
  }

  vec3 sample_square() const {
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit
    // square.
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  point3 defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

  color ray_color(const ray &r, int depth, const hittable &world, color throughput = color(1, 1, 1)) const {
    if (depth <= 0)
      return color(0, 0, 0);

    hit_record rec;

    if (!world.hit(r, interval(0.001, infinity), rec)) {
      if (background.x() >= 0.0) {
        return background;
      }
      vec3 unit_direction = unit_vector(r.direction());
      auto a = 0.5 * (unit_direction.y() + 1.0);
      return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    color color_from_emission = rec.mat->emitted();
    if (color_from_emission.length_squared() > 0) {
      return color_from_emission;
    }

    ray scattered;
    color attenuation;

    if (!rec.mat->scatter(r, rec, attenuation, scattered))
      return color_from_emission;

    color direct_light(0, 0, 0);

    if (lights && !lights->objects.empty()) {
      int num_lights = static_cast<int>(lights->objects.size());
      int idx = random_int(0, num_lights - 1);
      auto light = lights->objects[idx];

      aabb bbox = light->bounding_box();
      point3 light_center((bbox.x.min + bbox.x.max) * 0.5,
                          (bbox.y.min + bbox.y.max) * 0.5,
                          (bbox.z.min + bbox.z.max) * 0.5);
      double rx = (bbox.x.max - bbox.x.min) * 0.5;
      double ry = (bbox.y.max - bbox.y.min) * 0.5;
      double rz = (bbox.z.max - bbox.z.min) * 0.5;
      double approx_radius = std::fmax(rx, std::fmax(ry, rz));

      point3 light_pos = light_center + random_unit_vector() * approx_radius;
      vec3 light_dir = light_pos - rec.p;
      double light_dist = light_dir.length();
      vec3 light_dir_u = unit_vector(light_dir);

      ray shadow_ray(rec.p, light_dir_u, r.time());

      static thread_local shadow_cache cache;

      if (!hit_shadow(world, shadow_ray, interval(0.001, light_dist - 0.001),
                      cache)) {
        double cos_theta = std::fmax(0.0, dot(rec.normal, light_dir_u));
        if (cos_theta > 0.0) {
          color light_emit = light->get_emission();
          double r_sq = approx_radius * approx_radius;
          direct_light = attenuation * light_emit * cos_theta *
                         (r_sq * double(num_lights) / (light_dist * light_dist));
        }
      }
    }

    color next_throughput = throughput * attenuation;

    if (depth < max_depth - 4) {
      double p = std::fmax(next_throughput.x(), std::fmax(next_throughput.y(), next_throughput.z()));
      if (p < 0.2) {
        double survival_prob = std::fmax(p, 0.05);
        if (random_double() > survival_prob) {
          return color_from_emission + direct_light;
        }
        attenuation = attenuation / survival_prob;
        next_throughput = next_throughput / survival_prob;
      }
    }

    color color_from_scatter =
        attenuation * ray_color(scattered, depth - 1, world, next_throughput);

    return color_from_emission + direct_light + color_from_scatter;
  }
};

#endif