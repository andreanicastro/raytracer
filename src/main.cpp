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


std::shared_ptr<hitable> random_scene()
{
  int n = 50000;
  std::shared_ptr<hitable> list[n+1];
  std::shared_ptr<texture> checker = checker_texture::ptr( 
      constant_texture::ptr(Eigen::Vector3f(0.2,0.3,0.1)),
      constant_texture::ptr(Eigen::Vector3f(0.9, 0.9,0.9)));
  list[0] = sphere::ptr(Eigen::Vector3f(0, -1000,0), 1000, 
      lambertian::ptr(checker));
  int i = 1;
  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) { 
      float choose_mat = drand48();
      Eigen::Vector3f centre(a + 0.9*drand48(), 0.2, b+0.9*drand48());
      if ((centre - Eigen::Vector3f(4, 0.2, 0)).norm() > 0.9) {
        if (choose_mat < 0.8) { // diffuse
          list[i++] = moving_sphere::ptr(
              centre,
              centre + Eigen::Vector3f(0,0.5*drand48(),0),
              0.0, 1.0, 0.2,
              lambertian::ptr(constant_texture::ptr(Eigen::Vector3f(drand48()*drand48(),
                                                    drand48()*drand48(),
                                                    drand48()*drand48()))));
        } else if (choose_mat < 0.95) { // metal
          list[i++] = sphere::ptr(centre, 0.2,
              metal::ptr(Eigen::Vector3f(
                  0.5*(1 + drand48()), 
                  0.5*(1 + drand48()), 
                  0.5*(1 + drand48())), 0.5*(1+drand48())));
        } else { // glass
          list[i++] = sphere::ptr(centre, 0.2, dielectric::ptr(1.5));
        }
      }
    }
  }
  list[i++] = sphere::ptr(Eigen::Vector3f(0,1,0), 1.0, dielectric::ptr(1.5));
  list[i++] = sphere::ptr(Eigen::Vector3f(-4,1,0), 1.0, 
      lambertian::ptr(constant_texture::ptr(Eigen::Vector3f(0.4,0.2,0.1))));
  list[i++] = sphere::ptr(Eigen::Vector3f(4,1,0), 1.0, metal::ptr(Eigen::Vector3f(0.7,0.6,0.5), 0.0));
  return hitable_list::ptr(list, i);
}

std::shared_ptr<hitable> two_spheres() {
  std::shared_ptr<texture> checker = checker_texture::ptr(
      constant_texture::ptr(Eigen::Vector3f(0.2,0.3,0.1)),
      constant_texture::ptr(Eigen::Vector3f(0.9, 0.9, 0.9)));
  int n = 50;
  std::shared_ptr<hitable> list[n + 1];
  list[0] = sphere::ptr(Eigen::Vector3f(0, -10, 0), 10, lambertian::ptr(checker));
  list[1] = sphere::ptr(Eigen::Vector3f(0,  10, 0), 10, lambertian::ptr(checker));
  return hitable_list::ptr(list, 2);
}
            
std::shared_ptr<hitable> two_perlin_spheres() {
  std::shared_ptr<texture> pertext = noise_texture::ptr(4);
  std::shared_ptr<hitable> list[2];
  list[0] = sphere::ptr(Eigen::Vector3f(0,-1000, 0), 1000, lambertian::ptr(pertext));
  list[1] = sphere::ptr(Eigen::Vector3f(0, 2, 0), 2, lambertian::ptr(pertext));
  return hitable_list::ptr(list,2);
}

std::shared_ptr<hitable> one_earth(std::string texturePath) {
  int nx, ny, nn;
  unsigned char* tex_data = stbi_load(texturePath.c_str(), &nx, &ny, &nn, 0);
  std::shared_ptr<material> mat = lambertian::ptr(image_texture::ptr(tex_data, nx, ny));
  std::shared_ptr<hitable> list[1];
  list[0] = sphere::ptr(Eigen::Vector3f(0, 0, 0), 1, mat);
  return hitable_list::ptr(list, 1);
}

hitable* simple_light() {
  std::shared_ptr<texture> pertex = noise_texture::ptr(4);
  std::shared_ptr<hitable> list[4];
  list[0] = sphere::ptr(Eigen::Vector3f(0, -1000, 0), 1000, lambertian::ptr(pertex));
  list[1] = sphere::ptr(Eigen::Vector3f(0, 2, 0), 2, lambertian::ptr(pertex));
  list[2] = sphere::ptr(Eigen::Vector3f(0, 7, 0), 2, diffuse_light::ptr(
        constant_texture::ptr(Eigen::Vector3f(4,4,4))));
  list[3] =  xy_rect::ptr(3, 5, 1, 3, -2, diffuse_light::ptr(
        constant_texture::ptr(Eigen::Vector3f(4, 4, 4))));
  return hitable_list::ptr(list, 4);
}

std::shared_ptr<hitable> cornell_box() {
  std::shared_ptr<hitable> list[6];
  int i = 0;
  std::shared_ptr<material> red = lambertian::ptr( 
                    constant_texture::ptr(Eigen::Vector3f(0.65, 0.05, 0.05)));
  std::shared_ptr<material> white = lambertian::ptr( 
                      constant_texture::ptr(Eigen::Vector3f(0.73, 0.73, 0.73)));
  std::shared_ptr<material> green = lambertian::ptr( 
                      constant_texture::ptr(Eigen::Vector3f(0.12, 0.45, 0.15)));
  std::shared_ptr<material> light = diffuse_light::ptr(constant_texture::ptr(Eigen::Vector3f(15, 15, 15)));
  list[i++] = flip_normals::ptr(new yz_rect(0, 555, 0, 555, 555, green));
  list[i++] = yz_rect::ptr(0, 555, 0, 555, 0, red);
  list[i++] = xz_rect::ptr(213, 343, 227, 332, 554, light);
  list[i++] = flip_normals::ptr(new xz_rect(0, 555, 0, 555, 555, white));
  list[i++] = xz_rect::ptr(0, 555, 0, 555, 0, white);
  list[i++] = flip_normals::ptr(new xy_rect(0, 555, 0, 555, 555, white));
  list[i++] = translate::ptr(rotate_y::ptr(
        box::ptr(Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(165, 165, 165), white),
        -18), Eigen::Vector3f(130, 0, 65));
  list[i++] = translate::ptr(rotate_y::ptr( 
        new box(Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(165, 330, 165), white),
        15), Eigen::Vector3f(265,0,295));
  return hitable_list::ptr(list, i);
}

std::shared_ptr<hitable> cornell_smoke() {
  std::shared_ptr<hitable> list[8];
  int i = 0;
  std::shared_ptr<material> red = lambertian::ptr(
                    constant_texture::ptr(Eigen::Vector3f(0.65, 0.05, 0.05)));
  std::shared_ptr<material> white = lambertian::ptr(
                      constant_texture::ptr(Eigen::Vector3f(0.73, 0.73, 0.73)));
  std::shared_ptr<material> green = lambertian::ptr(
                      constant_texture::ptr(Eigen::Vector3f(0.12, 0.45, 0.15)));
  std::shared_ptr<material> light = diffuse_light::ptr(
                      constant_texture::ptr(Eigen::Vector3f(15, 15, 15)));
  list[i++] = flip_normals(yz_rect::ptr(0, 555, 0, 555, 555, green));
  list[i++] = yz_rect::ptr(0, 555, 0, 555,0, red);
  list[i++] = xz_rect::ptr(213, 343, 227, 332, 554, light);
  list[i++] = flip_normals::ptr(xz_rect::ptr(0, 555, 0, 555, 555, white));
  list[i++] = xz_rect::ptr(0, 555, 0, 555, 0, white);
  list[i++] = flip_normals::ptr(xy_rect::ptr(0, 555, 0, 555, 555, white));
  std::shared_ptr<hitable> b1 = translate::ptr(
      rotate_y::ptr(
        box::ptr(Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(165, 165, 165), white), 
        -18),
      Eigen::Vector3f(130, 0, 65));
  std::shared_ptr<hitable> b2 = translate::ptr(
                 rotate_y::ptr(
                  box::ptr( Eigen::Vector3f(0,0,0), Eigen::Vector3f(165, 330, 165), white), 15), 
                 Eigen::Vector3f(265, 0, 295));
  list[i++] =  constant_medium::ptr(b1, 0.01, 
                                   constant_texture::ptr(Eigen::Vector3f(1.0, 1.0, 1.0)));
  list[i++] =  constant_medium::ptr(b2, 0.01, 
                                   constant_texture::ptr(Eigen::Vector3f(0.0, 0.0, 0.0)));
  return hitable_list::ptr(list, i);
}

hitable* final_scene() {
  int nb = 20;
  std::shared_ptr<hitable> list[30];
  hitable** boxlist = new hitable*[10000];

  hitable** boxlist2  = new hitable*[10000];
  std::shared_ptr<material> white = lambertian::ptr(
                      constant_texture::ptr(Eigen::Vector3f(0.73, 0.73, 0.73)));
  std::shared_ptr<material> ground = lambertian::ptr(
                        constant_texture::ptr(Eigen::Vector3f(0.48, 0.83, 0.53)));
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
  std::shared_ptr<material> light = diffuse_light::ptr(constant_texture::ptr(Eigen::Vector3f(7,7,7)));
  list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
  Eigen::Vector3f center(400, 400, 200);
  list[l++] = new moving_sphere(center, center + Eigen::Vector3f(30, 0, 0), 0, 1, 50, 
      lambertian::ptr(constant_texture::ptr(Eigen::Vector3f(0.7, 0.3, 0.1))));

  list[l++] = new sphere(Eigen::Vector3f(260, 150, 45), 50, dielectric::ptr(1.5));
  list[l++] = new sphere(Eigen::Vector3f(0, 150, 145), 50, 
                         metal::ptr(Eigen::Vector3f(0.8, 0.8, 0.9), 10.0));
  hitable* boundary = new sphere(Eigen::Vector3f(360, 150, 145), 70, dielectric::ptr(1.5));
  list[l++] = boundary;
  list[l++] = new constant_medium(boundary, 0.2, 
                                  constant_texture::ptr(Eigen::Vector3f(0.2, 0.4, 0.9)));
  boundary = new sphere(Eigen::Vector3f(0, 0, 0), 5000, dielectric::ptr(1.5));
  list[l++] = new constant_medium(boundary, 0.0001, 
                                  constant_texture::ptr(Eigen::Vector3f(1.0, 1.0, 1.0)));
  int nx, ny, nz;
  unsigned char* tex_data = stbi_load("earth.jpeg", &nx, &ny, &nz, 0);
  std::shared_ptr<material> emat = lambertian::ptr(image_texture::ptr(tex_data, nx, ny));
  list[l++] = new sphere(Eigen::Vector3f(400,200, 400), 100, emat);
  std::shared_ptr<texture> pertex = noise_texture::ptr(0.1);
  list[l++] = new sphere(Eigen::Vector3f(220, 280, 300), 80, lambertian::ptr(pertex));
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

