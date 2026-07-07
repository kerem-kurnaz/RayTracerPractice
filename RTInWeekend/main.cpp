#include "rtweekend.h"
#include <string>
#include <cstdlib>

// Global settings override: set to > 0 to enforce the same sample count across all scenes.
// Set to -1 to use each scene's custom sample settings.
int global_samples_override = -1;

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "light.h"
#include "obj_loader.h"
#include "rotate.h"
#include "scale.h"
#include "sphere.h"
#include "translate.h"
#include "triangle.h"

void add_quad(hittable_list& world, point3 q0, vec3 u, vec3 v, shared_ptr<material> mat) {
  world.add(make_shared<triangle>(q0, q0+u, q0+u+v, mat));
  world.add(make_shared<triangle>(q0, q0+u+v, q0+v, mat));
}

void scene_showcase(hittable_list &world, camera &cam) {
  auto ground_material = make_shared<lambertian>(color(0.15, 0.15, 0.15));
  auto gold_material = make_shared<metal>(color(0.85, 0.64, 0.125), 0.05);
  auto glass_material = make_shared<dielectric>(1.5);
  auto red_material = make_shared<lambertian>(color(0.8, 0.15, 0.15));

  auto blue_metal = make_shared<metal>(color(0.2, 0.4, 0.9), 0.1);
  auto green_matte = make_shared<lambertian>(color(0.1, 0.7, 0.2));
  auto copper_metal = make_shared<metal>(color(0.85, 0.45, 0.25), 0.2);

  auto warm_light = make_shared<diffuse_light>(color(12.0, 9.0, 5.0));
  auto cool_light = make_shared<diffuse_light>(color(4.0, 7.0, 12.0));
  auto green_light = make_shared<diffuse_light>(color(0.0, 8.0, 2.0));
  auto pink_light = make_shared<diffuse_light>(color(8.0, 1.0, 4.0));

  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  hittable_list teapot_gold =
      load_obj("images-models/utah_teapot.obj", gold_material, 0.5,
               point3(-2.5, 0.9, -1.0), -90.0, 0.0, 0.0);
  world.add(make_shared<bvh_node>(teapot_gold));

  hittable_list teapot_glass =
      load_obj("images-models/utah_teapot.obj", glass_material, 0.3,
               point3(2.5, 0.9, -1.0), -90.0, 0.0, 0.0);
  world.add(make_shared<bvh_node>(teapot_glass));

  hittable_list teapot_red =
      load_obj("images-models/utah_teapot.obj", red_material, 0.6,
               point3(0.0, 0.9, 0.5), -90.0, 180.0, 0.0);
  world.add(make_shared<bvh_node>(teapot_red));

  world.add(make_shared<sphere>(point3(0.0, 1.0, 3.2), 1.0, glass_material));

  world.add(make_shared<sphere>(point3(-1.5, 0.4, 1.8), 0.4, blue_metal));
  world.add(make_shared<sphere>(point3(1.5, 0.4, 1.8), 0.4, green_matte));
  world.add(make_shared<sphere>(point3(-3.5, 0.6, 2.0), 0.6, copper_metal));

  auto l1 = make_shared<sphere>(point3(-5, 8, -5), 2.0, warm_light);
  auto l2 = make_shared<sphere>(point3(5, 8, 2), 1.5, cool_light);
  auto l3 = make_shared<sphere>(point3(0, 4, -1), 0.5, green_light);
  auto l4 = make_shared<sphere>(point3(-3.0, 3.5, 2.5), 0.4, pink_light);

  world.add(l1);
  world.add(l2);
  world.add(l3);
  world.add(l4);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(l1);
  cam.lights->add(l2);
  cam.lights->add(l3);
  cam.lights->add(l4);

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 20;
  cam.max_depth = 50;
  cam.vfov = 40;
  cam.lookfrom = point3(0, 6, 12);
  cam.lookat = point3(0, 1.2, 0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}

void scene_triangles(hittable_list &world, camera &cam) {
  auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
  auto teapot_material = make_shared<metal>(color(0.8, 0.6, 0.2), 0.1);
  auto light_material = make_shared<diffuse_light>(color(12.0, 12.0, 12.0));

  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  hittable_list teapot_model =
      load_obj("images-models/utah_teapot.obj", teapot_material, 1.5,
               point3(0, 1.0, 0), -90.0);
  world.add(make_shared<bvh_node>(teapot_model));

  auto l1 = make_shared<sphere>(point3(0, 8, -2), 2.0, light_material);
  world.add(l1);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(l1);

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 15;
  cam.max_depth = 50;
  cam.vfov = 40;
  cam.lookfrom = point3(0, 6, 12);
  cam.lookat = point3(0, 1.5, 0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}
void scene_dragon(hittable_list &world, camera &cam) {
  auto ground_material = make_shared<lambertian>(color(0.15, 0.15, 0.15));
  auto jade_material = make_shared<lambertian>(color(0.2, 0.6, 0.35));

  auto warm_light = make_shared<diffuse_light>(color(12.0, 9.0, 5.0));
  auto cool_light = make_shared<diffuse_light>(color(4.0, 7.0, 12.0));

  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  hittable_list dragon_model =
      load_obj("images-models/dragon.obj", jade_material, 6.0,
               point3(0.0, 1.6, 0.0), 0.0, 40.0, 0.0);
  world.add(make_shared<bvh_node>(dragon_model));

  auto l1 = make_shared<sphere>(point3(-5, 8, -5), 2.0, warm_light);
  auto l2 = make_shared<sphere>(point3(5, 8, 2), 1.5, cool_light);
  world.add(l1);
  world.add(l2);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(l1);
  cam.lights->add(l2);

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 20;
  cam.max_depth = 50;
  cam.vfov = 35;
  cam.lookfrom = point3(0, 5, 10);
  cam.lookat = point3(0, 1.2, 0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}
void scene_gallery(hittable_list &world, camera &cam) {
  auto ground_material = make_shared<lambertian>(color(0.15, 0.15, 0.15));
  auto gold_material = make_shared<metal>(color(0.85, 0.64, 0.125), 0.03);
  auto glass_material = make_shared<dielectric>(1.5);
  auto red_material = make_shared<lambertian>(color(0.7, 0.1, 0.1));
  auto jade_material = make_shared<lambertian>(color(0.15, 0.6, 0.3));

  auto warm_light = make_shared<diffuse_light>(color(10.0, 8.0, 4.0));
  auto cool_light = make_shared<diffuse_light>(color(4.0, 7.0, 10.0));
  auto green_light = make_shared<diffuse_light>(color(0.0, 6.0, 2.0));
  auto pink_light = make_shared<diffuse_light>(color(6.0, 1.0, 4.0));
  auto blue_light = make_shared<diffuse_light>(color(1.0, 4.0, 8.0));

  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  // 1. Stanford Dragon (870,000 triangles)
  hittable_list dragon_model =
      load_obj("images-models/dragon.obj", jade_material, 6.0,
               point3(0.0, 1.6, 0.0), 0.0, 45.0, 0.0);
  world.add(make_shared<bvh_node>(dragon_model));

  // 2. Gold Teapot
  hittable_list teapot_gold =
      load_obj("images-models/utah_teapot.obj", gold_material, 0.7,
               point3(-3.5, 0.5, -1.0), -90.0, 0.0, 0.0);
  world.add(make_shared<bvh_node>(teapot_gold));

  // 3. Glass Teapot
  hittable_list teapot_glass =
      load_obj("images-models/utah_teapot.obj", glass_material, 0.6,
               point3(3.5, 0.5, -1.0), -90.0, 0.0, 0.0);
  world.add(make_shared<bvh_node>(teapot_glass));

  // 4. Red Teapot
  hittable_list teapot_red =
      load_obj("images-models/utah_teapot.obj", red_material, 0.7,
               point3(0.0, 0.5, -4.5), -90.0, 180.0, 0.0);
  world.add(make_shared<bvh_node>(teapot_red));

  // 5. Five Light Sources
  auto l1 = make_shared<sphere>(point3(-6, 10, -6), 1.5, warm_light);
  auto l2 = make_shared<sphere>(point3(6, 10, -6), 1.5, cool_light);
  auto l3 = make_shared<sphere>(point3(0, 8, -4), 0.8, green_light);
  auto l4 = make_shared<sphere>(point3(-4, 5, 4), 0.6, pink_light);
  auto l5 = make_shared<sphere>(point3(4, 5, 4), 0.6, blue_light);

  world.add(l1);
  world.add(l2);
  world.add(l3);
  world.add(l4);
  world.add(l5);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(l1);
  cam.lights->add(l2);
  cam.lights->add(l3);
  cam.lights->add(l4);
  cam.lights->add(l5);

  // 6. Ring of 16 spheres around the center dragon
  for (int i = 0; i < 16; ++i) {
    double angle = i * (2.0 * 3.141592653589793 / 16.0);
    double x = 5.0 * cos(angle);
    double z = 5.0 * sin(angle);
    point3 center(x, 0.4, z);

    shared_ptr<material> mat;
    int choice = i % 4;
    if (choice == 0) {
      color col(0.1 + 0.05 * i, 0.9 - 0.05 * i, 0.3 + 0.02 * i);
      mat = make_shared<lambertian>(col);
    } else if (choice == 1) {
      color col(0.7 + 0.01 * i, 0.7 - 0.01 * i, 0.7 + 0.02 * i);
      mat = make_shared<metal>(col, 0.05 * (i % 5));
    } else if (choice == 2) {
      mat = make_shared<dielectric>(1.5);
    } else {
      mat = make_shared<metal>(color(0.85, 0.55, 0.25), 0.1);
    }
    world.add(make_shared<sphere>(center, 0.4, mat));
  }

  // 7. Six foreground spheres
  world.add(make_shared<sphere>(point3(-2.0, 0.3, 3.0), 0.3,
                                make_shared<dielectric>(1.5)));
  world.add(make_shared<sphere>(point3(-1.0, 0.3, 3.5), 0.3,
                                make_shared<metal>(color(0.9, 0.9, 0.9), 0.0)));
  world.add(make_shared<sphere>(point3(0.0, 0.3, 4.0), 0.3,
                                make_shared<lambertian>(color(0.9, 0.4, 0.1))));
  world.add(make_shared<sphere>(point3(1.0, 0.3, 3.5), 0.3,
                                make_shared<dielectric>(1.5)));
  world.add(make_shared<sphere>(point3(2.0, 0.3, 3.0), 0.3,
                                make_shared<metal>(color(0.2, 0.8, 0.8), 0.2)));
  world.add(make_shared<sphere>(point3(0.0, 0.2, 5.0), 0.2,
                                make_shared<lambertian>(color(0.1, 0.2, 0.9))));

  // 8. Dynamic field of 300+ tiny spheres of various materials and heights
  for (int x_idx = -12; x_idx <= 12; ++x_idx) {
    for (int z_idx = -12; z_idx <= 12; ++z_idx) {
      double x = x_idx * 0.85 + random_double(-0.15, 0.15);
      double z = z_idx * 0.85 + random_double(-0.15, 0.15);
      double r = random_double(0.08, 0.25);
      double y = random_double(r, 5.0);
      point3 pos(x, y, z);

      // Avoid spawning too close to the main models in 3D
      bool too_close = false;
      if ((pos - point3(0, 1.6, 0)).length() < 2.8) too_close = true;
      if ((pos - point3(-3.5, 0.9, -1)).length() < 1.8) too_close = true;
      if ((pos - point3(3.5, 0.9, -1)).length() < 1.8) too_close = true;
      if ((pos - point3(0, 0.9, -4.5)).length() < 1.8) too_close = true;
      if (z > 2.0 && std::fabs(x) < 3.2 && y < 2.0) too_close = true;

      if (!too_close) {
        shared_ptr<material> mat;
        double choose_mat = random_double();
        if (choose_mat < 0.4) {
          mat = make_shared<lambertian>(color::random());
        } else if (choose_mat < 0.7) {
          mat = make_shared<dielectric>(1.5);
        } else {
          mat = make_shared<metal>(color::random(), random_double(0.0, 0.3));
        }
        world.add(make_shared<sphere>(pos, r, mat));
      }
    }
  }

  // Camera Settings
  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 50;
  cam.max_depth = 50;
  cam.vfov = 35;
  cam.lookfrom = point3(0, 6, 13);
  cam.lookat = point3(0, 1.2, 0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}

void scene_light_transport(hittable_list &world, camera &cam) {
  auto ground_material = make_shared<lambertian>(color(0.2, 0.2, 0.2));
  auto gold_material = make_shared<metal>(color(0.85, 0.64, 0.125), 0.03);
  auto glass_material = make_shared<dielectric>(1.5);
  auto red_material = make_shared<lambertian>(color(0.8, 0.05, 0.05));
  auto green_material = make_shared<lambertian>(color(0.05, 0.8, 0.05));
  auto blue_material = make_shared<lambertian>(color(0.05, 0.05, 0.8));
  auto mirror_material = make_shared<metal>(color(0.95, 0.95, 0.95), 0.0);
  auto white_material = make_shared<lambertian>(color(0.8, 0.8, 0.8));
  auto orange_metal = make_shared<metal>(color(0.9, 0.4, 0.1), 0.1);
  auto purple_metal = make_shared<metal>(color(0.6, 0.1, 0.9), 0.1);
  auto yellow_material = make_shared<lambertian>(color(0.85, 0.85, 0.05));

  auto overhead_light = make_shared<diffuse_light>(color(8.0, 7.0, 5.0));
  auto left_warm_light = make_shared<diffuse_light>(color(12.0, 6.0, 2.0));
  auto right_cool_light = make_shared<diffuse_light>(color(2.0, 6.0, 12.0));

  // 1. Ground Plane
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  // 2. Red Teapot
  hittable_list teapot_red =
      load_obj("images-models/utah_teapot.obj", red_material, 0.6,
               point3(-1.2, 0.4, -0.8), -90.0, 0.0, 0.0);
  world.add(make_shared<bvh_node>(teapot_red));

  // 3. Gold Teapot
  hittable_list teapot_gold =
      load_obj("images-models/utah_teapot.obj", gold_material, 0.6,
               point3(1.2, 0.4, -0.8), -90.0, 0.0, 0.0);
  world.add(make_shared<bvh_node>(teapot_gold));

  // 4. Glass Dragon
  hittable_list dragon_glass =
      load_obj("images-models/dragon.obj", glass_material, 4.0,
               point3(0.0, 1.1, 0.8), 0.0, 45.0, 0.0);
  world.add(make_shared<bvh_node>(dragon_glass));

  // 5. Overhead Light
  auto l1 = make_shared<sphere>(point3(0, 5, 0), 1.0, overhead_light);
  // 6. Warm Side Light
  auto l2 = make_shared<sphere>(point3(-3.0, 3.0, 2.0), 0.5, left_warm_light);
  // 7. Cool Side Light
  auto l3 = make_shared<sphere>(point3(3.0, 3.0, 2.0), 0.5, right_cool_light);

  world.add(l1);
  world.add(l2);
  world.add(l3);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(l1);
  cam.lights->add(l2);
  cam.lights->add(l3);

  // 8. Green Matte Sphere
  world.add(make_shared<sphere>(point3(-0.9, 0.3, 2.0), 0.3, green_material));
  // 9. Blue Matte Sphere
  world.add(make_shared<sphere>(point3(0.9, 0.3, 2.0), 0.3, blue_material));
  // 10. Glass Sphere (Near Red Teapot)
  world.add(make_shared<sphere>(point3(-1.8, 0.4, 0.8), 0.4, glass_material));
  // 11. Mirror Sphere (Near Gold Teapot)
  world.add(make_shared<sphere>(point3(1.8, 0.4, 0.8), 0.4, mirror_material));
  // 12. White Matte Sphere (In shadow behind dragon)
  world.add(make_shared<sphere>(point3(0.0, 0.35, -2.2), 0.35, white_material));
  // 13. Orange Metal Sphere
  world.add(make_shared<sphere>(point3(-2.2, 0.3, -1.8), 0.3, orange_metal));
  // 14. Purple Metal Sphere
  world.add(make_shared<sphere>(point3(2.2, 0.3, -1.8), 0.3, purple_metal));
  // 15. Yellow Matte Sphere (Foreground Center)
  world.add(make_shared<sphere>(point3(0.0, 0.25, 2.5), 0.25, yellow_material));

  // Camera settings optimized for close-up viewing
  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 50;
  cam.max_depth = 50;
  cam.vfov = 35;
  cam.lookfrom = point3(0, 3.5, 7.5);
  cam.lookat = point3(0, 0.8, 0.0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}

void scene_infinity_mirror(hittable_list &world, camera &cam) {
  auto mirror_material = make_shared<metal>(color(0.88, 0.88, 0.88), 0.0);
  auto gold_material = make_shared<metal>(color(0.85, 0.64, 0.125), 0.03);
  auto jade_material = make_shared<lambertian>(color(0.15, 0.6, 0.3));
  auto light_material = make_shared<diffuse_light>(color(3.0, 2.5, 1.5));

  // Build mirror room walls (completely enclosed box)
  add_quad(world, point3(-5, 0, -5), vec3(10, 0, 0), vec3(0, 0, 10), mirror_material);
  add_quad(world, point3(-5, 8, -5), vec3(10, 0, 0), vec3(0, 0, 10), mirror_material);
  add_quad(world, point3(-5, 0, -5), vec3(10, 0, 0), vec3(0, 8, 0), mirror_material);
  add_quad(world, point3(-5, 0, 5), vec3(10, 0, 0), vec3(0, 8, 0), mirror_material);
  add_quad(world, point3(-5, 0, -5), vec3(0, 0, 10), vec3(0, 8, 0), mirror_material);
  add_quad(world, point3(5, 0, -5), vec3(0, 0, 10), vec3(0, 8, 0), mirror_material);

  hittable_list teapot = load_obj("images-models/utah_teapot.obj", gold_material, 0.65, point3(-1.5, 0.0, -1.0), -90.0, 0.0, 0.0);
  hittable_list dragon = load_obj("images-models/dragon.obj", jade_material, 4.5, point3(1.5, 0.0, -1.0), 0.0, 45.0, 0.0);
  world.add(make_shared<bvh_node>(teapot));
  world.add(make_shared<bvh_node>(dragon));

  auto light = make_shared<sphere>(point3(0.0, 4.0, 0.0), 0.6, light_material);
  world.add(light);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(light);

  cam.background = color(0.01, 0.01, 0.01);
  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 50;
  cam.max_depth = 50;
  cam.vfov = 50;
  cam.lookfrom = point3(0, 3.0, 4.0);
  cam.lookat = point3(0, 2.0, -1.0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}

void scene_single_mirror(hittable_list &world, camera &cam) {
  auto mirror_material = make_shared<metal>(color(0.98, 0.98, 0.98), 0.0);
  auto gold_material = make_shared<metal>(color(0.85, 0.64, 0.125), 0.03);
  auto jade_material = make_shared<lambertian>(color(0.15, 0.6, 0.3));
  auto white_wall = make_shared<lambertian>(color(0.73, 0.73, 0.73));
  auto light_material = make_shared<diffuse_light>(color(8.0, 7.0, 5.0));

  // Floor, ceiling, left, and right walls are diffuse white. Only back wall is a mirror!
  add_quad(world, point3(-5, 0, -5), vec3(10, 0, 0), vec3(0, 0, 10), white_wall);
  add_quad(world, point3(-5, 8, -5), vec3(10, 0, 0), vec3(0, 0, 10), white_wall);
  add_quad(world, point3(-5, 0, -5), vec3(0, 0, 10), vec3(0, 8, 0), white_wall);
  add_quad(world, point3(5, 0, -5), vec3(0, 0, 10), vec3(0, 8, 0), white_wall);
  
  // Back Wall is a single perfect mirror reflecting the scene
  add_quad(world, point3(-5, 0, -5), vec3(10, 0, 0), vec3(0, 8, 0), mirror_material);

  hittable_list teapot = load_obj("images-models/utah_teapot.obj", gold_material, 0.65, point3(-1.5, 0.0, -1.0), -90.0, 0.0, 0.0);
  hittable_list dragon = load_obj("images-models/dragon.obj", jade_material, 4.5, point3(1.5, 0.0, -1.0), 0.0, 45.0, 0.0);
  world.add(make_shared<bvh_node>(teapot));
  world.add(make_shared<bvh_node>(dragon));

  auto light = make_shared<sphere>(point3(0.0, 4.0, 0.0), 0.6, light_material);
  world.add(light);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(light);

  cam.background = color(0.1, 0.15, 0.2); // Soft sky color for rays exiting the front
  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 50;
  cam.max_depth = 50;
  cam.vfov = 50;
  cam.lookfrom = point3(0, 3.0, 4.5);
  cam.lookat = point3(0, 2.0, -1.0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}


void scene_orrery(hittable_list &world, camera &cam) {
  cam.background = color(0, 0, 0);

  auto sun_light = make_shared<diffuse_light>(color(15.0, 12.0, 8.0));
  auto earth_material = make_shared<lambertian>(color(0.2, 0.5, 0.8));
  auto planet_gold = make_shared<metal>(color(0.85, 0.65, 0.15), 0.1);
  auto ring_material = make_shared<dielectric>(1.5);
  auto red_planet = make_shared<lambertian>(color(0.7, 0.2, 0.15));

  auto sun = make_shared<sphere>(point3(0, 0, 0), 2.2, sun_light);
  world.add(sun);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(sun);

  world.add(make_shared<sphere>(point3(-4.5, 0.5, 3.0), 0.7, earth_material));

  world.add(make_shared<sphere>(point3(5.5, -0.2, -1.0), 0.9, planet_gold));
  for (int i = 0; i < 80; ++i) {
    double angle = i * (2.0 * 3.141592653589793 / 80.0);
    double x = 5.5 + 1.8 * cos(angle);
    double z = -1.0 + 1.8 * sin(angle);
    double y = -0.2 + 0.1 * sin(angle * 2.0);
    world.add(make_shared<sphere>(point3(x, y, z), 0.08, ring_material));
  }

  world.add(make_shared<sphere>(point3(-1.0, 1.2, -6.0), 0.5, red_planet));

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 50;
  cam.max_depth = 50;
  cam.vfov = 35;
  cam.lookfrom = point3(0, 8, 16);
  cam.lookat = point3(0, 0, 0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}

void scene_cornell_box(hittable_list &world, camera &cam) {
  cam.background = color(0, 0, 0);

  auto red_wall = make_shared<lambertian>(color(0.65, 0.05, 0.05));
  auto white_wall = make_shared<lambertian>(color(0.73, 0.73, 0.73));
  auto green_wall = make_shared<lambertian>(color(0.12, 0.45, 0.15));
  auto light = make_shared<diffuse_light>(color(15.0, 15.0, 15.0));

  auto gold_material = make_shared<metal>(color(0.85, 0.64, 0.125), 0.03);
  auto glass_material = make_shared<dielectric>(1.5);

  add_quad(world, point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green_wall);
  add_quad(world, point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red_wall);
  auto l1 = make_shared<sphere>(point3(278, 554, 278), 60.0, light);
  world.add(l1);
  
  cam.lights = make_shared<hittable_list>();
  cam.lights->add(l1);

  add_quad(world, point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white_wall);
  add_quad(world, point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white_wall);
  add_quad(world, point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white_wall);

  hittable_list teapot = load_obj("images-models/utah_teapot.obj", gold_material, 55.0, point3(180, 0, 150), -90.0, 25.0, 0.0);
  world.add(make_shared<bvh_node>(teapot));

  hittable_list dragon = load_obj("images-models/dragon.obj", glass_material, 550.0, point3(360, 120, 320), 0.0, 210.0, 0.0);
  world.add(make_shared<bvh_node>(dragon));

  world.add(make_shared<sphere>(point3(150, 60, 320), 60.0, glass_material));

  cam.aspect_ratio = 1.0;
  cam.image_width = 600;
  cam.samples_per_pixel = 120;
  cam.max_depth = 50;
  cam.vfov = 40;
  cam.lookfrom = point3(278, 278, -800);
  cam.lookat = point3(278, 278, 0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}

void scene_bokeh_canopy(hittable_list &world, camera &cam) {
  auto ground_material = make_shared<lambertian>(color(0.1, 0.1, 0.1));
  auto glass_material = make_shared<dielectric>(1.5);
  auto center_light = make_shared<diffuse_light>(color(25.0, 5.0, 5.0));

  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  auto light = make_shared<sphere>(point3(0.0, 2.5, 0.0), 0.7, center_light);
  world.add(light);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(light);

  for (int i = 0; i < 150; ++i) {
    double angle = i * 0.35;
    double r = 0.3 + 0.02 * i;
    double x = r * cos(angle);
    double z = r * sin(angle);
    double y = 0.5 + 0.03 * i;
    world.add(make_shared<sphere>(point3(x, y, z), 0.18, glass_material));
  }

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 50;
  cam.max_depth = 50;
  cam.vfov = 35;
  cam.lookfrom = point3(0, 1.2, 5.0);
  cam.lookat = point3(0, 1.2, 0.0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 8.0;
  cam.focus_dist = 4.8;
}

void scene_prism_chamber(hittable_list &world, camera &cam) {
  auto table_material = make_shared<lambertian>(color(0.85, 0.85, 0.85));
  auto glass_material = make_shared<dielectric>(1.5);

  auto red_light = make_shared<diffuse_light>(color(15.0, 0.0, 0.0));
  auto green_light = make_shared<diffuse_light>(color(0.0, 15.0, 0.0));
  auto blue_light = make_shared<diffuse_light>(color(0.0, 0.0, 15.0));

  add_quad(world, point3(-6, 0, -6), vec3(12, 0, 0), vec3(0, 0, 12), table_material);

  point3 b0(-1.0, 0.01, -0.6);
  point3 b1(1.0, 0.01, -0.6);
  point3 b2(0.0, 0.01, 1.0);

  point3 t0(-1.0, 2.0, -0.6);
  point3 t1(1.0, 2.0, -0.6);
  point3 t2(0.0, 2.0, 1.0);

  world.add(make_shared<triangle>(b0, b2, b1, glass_material));
  world.add(make_shared<triangle>(t0, t1, t2, glass_material));
  world.add(make_shared<triangle>(b0, b1, t1, glass_material));
  world.add(make_shared<triangle>(b0, t1, t0, glass_material));
  world.add(make_shared<triangle>(b1, b2, t2, glass_material));
  world.add(make_shared<triangle>(b1, t2, t1, glass_material));
  world.add(make_shared<triangle>(b2, b0, t0, glass_material));
  world.add(make_shared<triangle>(b2, t0, t2, glass_material));

  auto l1 = make_shared<sphere>(point3(-3, 3, -2), 0.4, red_light);
  auto l2 = make_shared<sphere>(point3(3, 3, -2), 0.4, green_light);
  auto l3 = make_shared<sphere>(point3(0, 3, 2), 0.4, blue_light);

  world.add(l1);
  world.add(l2);
  world.add(l3);

  cam.lights = make_shared<hittable_list>();
  cam.lights->add(l1);
  cam.lights->add(l2);
  cam.lights->add(l3);

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;
  cam.samples_per_pixel = 50;
  cam.max_depth = 50;
  cam.vfov = 35;
  cam.lookfrom = point3(0, 4.0, 6.0);
  cam.lookat = point3(0, 0.8, 0.0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.0;
  cam.focus_dist = 10.0;
}

void run_scene(int scene_select, const std::string& filename) {
  hittable_list world;
  camera cam;
  
  std::clog << "Configuring scene " << scene_select << "...\n";
  switch (scene_select) {
    case 1: scene_showcase(world, cam); break;
    case 2: scene_triangles(world, cam); break;
    case 3: scene_dragon(world, cam); break;
    case 4: scene_gallery(world, cam); break;
    case 5: scene_light_transport(world, cam); break;
    case 6: scene_infinity_mirror(world, cam); break;
    case 7: scene_orrery(world, cam); break;
    case 8: scene_cornell_box(world, cam); break;
    case 9: scene_bokeh_canopy(world, cam); break;
    case 10: scene_prism_chamber(world, cam); break;
    case 11: scene_single_mirror(world, cam); break;
    default: scene_light_transport(world, cam); break;
  }
  
  if (global_samples_override > 0) {
    cam.samples_per_pixel = global_samples_override;
  }
  
  std::clog << "Building BVH for scene " << scene_select << "...\n";
  world = hittable_list(make_shared<bvh_node>(world));
  std::clog << "Rendering scene " << scene_select << " to: " << filename << "\n";
  cam.render(world, filename);
}

int main(int argc, char* argv[]) {
  std::string arg = (argc > 1) ? argv[1] : "5";
  
  if (arg == "all") {
    std::clog << "Rendering all scenes...\n";
    run_scene(1, "images-models/showcase_studio.ppm");
    run_scene(2, "images-models/teapot_alone.ppm");
    run_scene(3, "images-models/dragon_alone.ppm");
    run_scene(4, "images-models/gallery_showcase.ppm");
    run_scene(5, "images-models/light_transport.ppm");
    run_scene(6, "images-models/infinity_mirror.ppm");
    run_scene(7, "images-models/planetary_orrery.ppm");
    run_scene(8, "images-models/cornell_box.ppm");
    run_scene(9, "images-models/bokeh_canopy.ppm");
    run_scene(10, "images-models/prism_chamber.ppm");
    run_scene(11, "images-models/single_mirror.ppm");
  } else {
    int scene = std::atoi(arg.c_str());
    std::string filename = "images-models/showcase_scene.ppm";
    switch (scene) {
      case 1: filename = "images-models/showcase_studio.ppm"; break;
      case 2: filename = "images-models/teapot_alone.ppm"; break;
      case 3: filename = "images-models/dragon_alone.ppm"; break;
      case 4: filename = "images-models/gallery_showcase.ppm"; break;
      case 5: filename = "images-models/light_transport.ppm"; break;
      case 6: filename = "images-models/infinity_mirror.ppm"; break;
      case 7: filename = "images-models/planetary_orrery.ppm"; break;
      case 8: filename = "images-models/cornell_box.ppm"; break;
      case 9: filename = "images-models/bokeh_canopy.ppm"; break;
      case 10: filename = "images-models/prism_chamber.ppm"; break;
      case 11: filename = "images-models/single_mirror.ppm"; break;
    }
    run_scene(scene, filename);
  }
  return 0;
}
