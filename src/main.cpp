#include <iostream>
#include <fstream>
#include <limits> 
#include <random>
#include <stdlib.h> 
#include <chrono>
#include "sphere.hpp"
#include "hitablelist.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "volume.hpp"
#include "rectangle.hpp"
#include "box.hpp"
#include "instance.hpp"
#include "bvh.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


hitable* random_scene()
{
  int n = 50000;
  hitable **list = new hitable*[n+1];
  texture* checker = new checker_texture( 
      new constant_texture(Eigen::Vector3f(0.2,0.3,0.1)),
      new constant_texture(Eigen::Vector3f(0.9, 0.9,0.9)));
  list[0] = new sphere(Eigen::Vector3f(0, -1000,0), 1000, 
      new lambertian(checker));
  int i = 1;
  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) { 
      float choose_mat = drand48();
      Eigen::Vector3f centre(a + 0.9*drand48(), 0.2, b+0.9*drand48());
      if ((centre - Eigen::Vector3f(4, 0.2, 0)).norm() > 0.9) {
        if (choose_mat < 0.8) { // diffuse
          list[i++] = new moving_sphere(
              centre,
              centre + Eigen::Vector3f(0,0.5*drand48(),0),
              0.0, 1.0, 0.2,
              new lambertian(new constant_texture(Eigen::Vector3f(drand48()*drand48(),
                                                       drand48()*drand48(),
                                                       drand48()*drand48()))));
        } else if (choose_mat < 0.95) { // metal
          list[i++] = new sphere(centre, 0.2,
              new metal(Eigen::Vector3f(
                  0.5*(1 + drand48()), 
                  0.5*(1 + drand48()), 
                  0.5*(1 + drand48())), 0.5*(1+drand48())));
        } else { // glass
          list[i++] = new sphere(centre, 0.2, new dielectric(1.5));
        }
      }
    }
  }
  list[i++] = new sphere(Eigen::Vector3f(0,1,0), 1.0, new dielectric(1.5));
  list[i++] = new sphere(Eigen::Vector3f(-4,1,0), 1.0, 
      new lambertian(new constant_texture(Eigen::Vector3f(0.4,0.2,0.1))));
  list[i++] = new sphere(Eigen::Vector3f(4,1,0), 1.0, new metal(Eigen::Vector3f(0.7,0.6,0.5), 0.0));
  return new hitable_list(list, i);
}

hitable* two_spheres() {
  texture* checker = new checker_texture(
      new constant_texture(Eigen::Vector3f(0.2,0.3,0.1)),
      new constant_texture(Eigen::Vector3f(0.9, 0.9, 0.9)));
  int n = 50;
  hitable** list = new hitable*[n+1];
  list[0] = new sphere(Eigen::Vector3f(0, -10, 0), 10, new lambertian(checker));
  list[1] = new sphere(Eigen::Vector3f(0,  10, 0), 10, new lambertian(checker));
  return new hitable_list(list, 2);
}
            
hitable* two_perlin_spheres() {
  texture* pertext = new noise_texture(4);
  hitable** list = new hitable*[2];
  list[0] = new sphere(Eigen::Vector3f(0,-1000, 0), 1000, new lambertian(pertext));
  list[1] = new sphere(Eigen::Vector3f(0, 2, 0), 2, new lambertian(pertext));
  return new hitable_list(list,2);
}

hitable* one_earth(std::string texturePath) {
  int nx, ny, nn;
  unsigned char* tex_data = stbi_load(texturePath.c_str(), &nx, &ny, &nn, 0);
  material* mat = new lambertian(new image_texture(tex_data, nx, ny));
  hitable** list = new hitable*[1];
  list[0] = new sphere(Eigen::Vector3f(0, 0, 0), 1, mat);
  return new hitable_list(list, 1);
}

hitable* simple_light() {
  texture* pertex = new noise_texture(4);
  hitable** list = new hitable*[4];
  list[0] = new sphere(Eigen::Vector3f(0, -1000, 0), 1000, new lambertian(pertex));
  list[1] = new sphere(Eigen::Vector3f(0, 2, 0), 2, new lambertian(pertex));
  list[2] = new sphere(Eigen::Vector3f(0, 7, 0), 2, new diffuse_light(
        new constant_texture(Eigen::Vector3f(4,4,4))));
  list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(
        new constant_texture(Eigen::Vector3f(4, 4, 4))));
  return new hitable_list(list, 4);
}

hitable* cornell_box() {
  hitable **list = new hitable*[6];
  int i = 0;
  material *red = new lambertian( 
                    new constant_texture(Eigen::Vector3f(0.65, 0.05, 0.05)));
  material *white = new lambertian( 
                      new constant_texture(Eigen::Vector3f(0.73, 0.73, 0.73)));
  material *green = new lambertian( 
                      new constant_texture(Eigen::Vector3f(0.12, 0.45, 0.15)));
  material *light = new diffuse_light(new constant_texture(Eigen::Vector3f(15, 15, 15)));
  list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
  list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
  list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
  list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
  list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
  list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
  list[i++] = new translate(new rotate_y(
        new box(Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(165, 165, 165), white),
        -18), Eigen::Vector3f(130, 0, 65));
  list[i++] = new translate(new rotate_y( 
        new box(Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(165, 330, 165), white),
        15), Eigen::Vector3f(265,0,295));
  return new hitable_list(list, i);
}

hitable* cornell_smoke() {
  hitable **list = new hitable*[8];
  int i = 0;
  material* red = new lambertian(
                    new constant_texture(Eigen::Vector3f(0.65, 0.05, 0.05)));
  material* white = new lambertian(
                      new constant_texture(Eigen::Vector3f(0.73, 0.73, 0.73)));
  material* green = new lambertian(
                      new constant_texture(Eigen::Vector3f(0.12, 0.45, 0.15)));
  material* light = new diffuse_light(
                      new constant_texture(Eigen::Vector3f(15, 15, 15)));
  list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
  list[i++] = new yz_rect(0, 555, 0, 555,0, red);
  list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
  list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
  list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
  list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
  hitable* b1 = new translate(
      new rotate_y(
        new box(Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(165, 165, 165), white), 
        -18),
      Eigen::Vector3f(130, 0, 65));
  hitable* b2 = new translate(
                 new rotate_y(
                  new box( Eigen::Vector3f(0,0,0), Eigen::Vector3f(165, 330, 165), white), 15), 
                 Eigen::Vector3f(265, 0, 295));
  list[i++] =  new constant_medium(b1, 0.01, 
                                   new constant_texture(Eigen::Vector3f(1.0, 1.0, 1.0)));
  list[i++] =  new constant_medium(b2, 0.01, 
                                   new constant_texture(Eigen::Vector3f(0.0, 0.0, 0.0)));
  return new hitable_list(list, i);
}

hitable* final_scene() {
  int nb = 20;
  hitable** list = new hitable*[30];
  hitable** boxlist = new hitable*[10000];
  hitable** boxlist2  = new hitable*[10000];
  material* white = new lambertian(
                      new constant_texture(Eigen::Vector3f(0.73, 0.73, 0.73)));
  material* ground = new lambertian(
                        new constant_texture(Eigen::Vector3f(0.48, 0.83, 0.53)));
  int b = 0;
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < nb; j++) {
      float w = 100;
      float x0 = -1000 + i * w;
      float z0 = -1000 + j * w;
      float y0 = 0;
      float x1 = x0 + w;
      float y1 = 100 * (drand48() + 0.01);
      float z1 = z0 + w;
      boxlist[b++] = new box(Eigen::Vector3f(x0, y0, z0), Eigen::Vector3f(x1, y1, z1), ground);
    }
  }
  int l = 0;
  list[l++] = new bvh_node(boxlist, b, 0, 1);
  material* light = new diffuse_light(new constant_texture(Eigen::Vector3f(7,7,7)));
  list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
  Eigen::Vector3f center(400, 400, 200);
  list[l++] = new moving_sphere(center, center + Eigen::Vector3f(30, 0, 0), 0, 1, 50, 
      new lambertian(new constant_texture(Eigen::Vector3f(0.7, 0.3, 0.1))));

  list[l++] = new sphere(Eigen::Vector3f(260, 150, 45), 50, new dielectric(1.5));
  list[l++] = new sphere(Eigen::Vector3f(0, 150, 145), 50, 
                         new metal(Eigen::Vector3f(0.8, 0.8, 0.9), 10.0));
  hitable* boundary = new sphere(Eigen::Vector3f(360, 150, 145), 70, new dielectric(1.5));
  list[l++] = boundary;
  list[l++] = new constant_medium(boundary, 0.2, 
                                  new constant_texture(Eigen::Vector3f(0.2, 0.4, 0.9)));
  boundary = new sphere(Eigen::Vector3f(0, 0, 0), 5000, new dielectric(1.5));
  list[l++] = new constant_medium(boundary, 0.0001, 
                                  new constant_texture(Eigen::Vector3f(1.0, 1.0, 1.0)));
  int nx, ny, nz;
  unsigned char* tex_data = stbi_load("earth.jpeg", &nx, &ny, &nz, 0);
  material *emat = new lambertian(new image_texture(tex_data, nx, ny));
  list[l++] = new sphere(Eigen::Vector3f(400,200, 400), 100, emat);
  texture *pertex = new noise_texture(0.1);
  list[l++] = new sphere(Eigen::Vector3f(220, 280, 300), 80, new lambertian(pertex));
  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxlist2[j] = new sphere(Eigen::Vector3f(165*drand48(), 165*drand48(), 165*drand48()),
                              10, white);
  }
  list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 
                                         15), Eigen::Vector3f(-100, 270, 395));
  return new hitable_list(list, l);
}
  
Eigen::Vector3f color(const ray& r, hitable *world, int depth) {
  hit_record rec;
  if (world->hit(r, 0.001, std::numeric_limits<float>::max(), rec)) {
    ray scattered;
    Eigen::Vector3f attenuation = Eigen::Vector3f::Zero();
    Eigen::Vector3f emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      Eigen::Vector3f col = color(scattered, world, depth+1);
      Eigen::Vector3f col_att = attenuation.array() * col.array();
      return emitted + col_att;
    }
    else
      return emitted;
  }
  else {
    return Eigen::Vector3f::Zero();
  }
}

int main() {
  int nx = 256;
  int ny = 192;
  int ns = 100;

 
  auto wc_start = std::chrono::high_resolution_clock::now();
  //hitable *world = random_scene();
  //hitable* world = two_spheres();
  //hitable* world = two_perlin_spheres();
  //hitable* world = one_earth("earth.jpeg");
  //hitable* world = simple_light();
  hitable* world = cornell_box();
  //hitable* world = cornell_smoke();
  //hitable* world = final_scene();
  auto wc_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> wc_elapsed = wc_end - wc_start;
  std::cout << "World creation: " << wc_elapsed.count() << std::endl;
  Eigen::Vector3f lookfrom(278, 178, -800);
  Eigen::Vector3f lookat(278, 278, 0);
  float dist_to_focus = 10.0;
  float aperture = 0.0;
  float vfov = 40.0;
  camera cam(lookfrom,lookat, Eigen::Vector3f(0,1,0), vfov, float(nx)/float(ny),
      aperture, dist_to_focus, 0.0, 1.0);
  auto render_begin = std::chrono::high_resolution_clock::now();
  std::ofstream outputfile;
  outputfile.open("./test.ppm");
  outputfile << "P3\n" << nx << " " << ny << "\n255\n";
  for (int j = ny - 1; j >= 0; j --) {
    for (int i = 0; i < nx; ++i) {
      Eigen::Vector3f col(0,0,0);
      for (int s=0; s < ns; ++s) {
        float u = float(i + drand48())/float(nx);
        float v = float(j + drand48())/float(ny);
        ray r = cam.get_ray(u,v);
        col += color(r, world,0);
      }
      col /= float(ns);
      col = Eigen::Vector3f(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
      int ir = int(255.99*col[0]);
      int ig = int(255.99*col[1]);
      int ib = int(255.99*col[2]);
      outputfile << ir << " " << ig << " " << ib << "\n";
    }
  }
  outputfile.close();
  auto render_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> render_elapsed = render_end - render_begin;
  std::cout << "Total rendering time: " << render_elapsed.count() << std::endl;

}

