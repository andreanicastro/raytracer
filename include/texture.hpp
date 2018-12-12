#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <Eigen/Core>
#include "perlin.hpp" 

class texture {
  public: 
    virtual Eigen::Vector3f value(float u, float v, const Eigen::Vector3f& p) const = 0;
};

class constant_texture : public texture {
  public:
    constant_texture() {}
    constant_texture(Eigen::Vector3f c) : color(c) {}
    virtual Eigen::Vector3f value(float u, float v, const Eigen::Vector3f& p) const {
      return color;
    }
    Eigen::Vector3f color;
};

class checker_texture : public texture { 
  public:
    checker_texture() {}
    checker_texture(texture *t0, texture *t1): even(t0), odd(t1) {}
    virtual Eigen::Vector3f value(float u, float v, const Eigen::Vector3f& p) const {
      float sines = sin(10*p.x())*sin(10*p.y())*sin(10*p.z());
      if (sines < 0) 
        return odd->value(u,v,p);
      else 
        return even->value(u,v,p);
    }

    texture* odd;
    texture* even;
};

class noise_texture : public texture {
  public: 
    noise_texture() {}
    noise_texture(float sc) : scale(sc) {}
    virtual Eigen::Vector3f value(float u, float v, const Eigen::Vector3f& p) const {
      //return Eigen::Vector3f(1, 1, 1) * 0.5 * (1 + noise.noise(scale * p));
      //return Eigen::Vector3f(1, 1, 1) * noise.turb(scale * p);
      return Eigen::Vector3f::Ones() * 0.5 * (1 + sin(scale * p[2] + 10 * noise.turb(p)));
    }
    perlin noise;
    float scale;
};

class image_texture : public texture {
  public:
    image_texture() {}
    image_texture(unsigned char* pixels, int A, int B) : 
      data(pixels), nx(A), ny(B) {}
    
    virtual Eigen::Vector3f value(float u, float v, const Eigen::Vector3f& p) const;
    unsigned char *data;
    int nx, ny;
};

Eigen::Vector3f image_texture::value(float u, float v, const Eigen::Vector3f& p) const {
  int i = (    u) * nx;
  int j = (1 - v) * ny - 0.001;
  if (i < 0) i = 0;
  if (j < 0) j = 0;
  if (i > nx - 1) i = nx - 1;
  if (j > nx - 1) j = ny - 1;
  float r = int(data[3 * i + 3 * nx * j    ]) / 255.0;
  float g = int(data[3 * i + 3 * nx * j + 1]) / 255.0;
  float b = int(data[3 * i + 3 * nx * j + 2]) / 255.0;
  return Eigen::Vector3f(r, g, b);
}


#endif //TEXTURE_HPP
