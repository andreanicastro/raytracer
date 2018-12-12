#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "ray.hpp"
#include "texture.hpp"
#include "hitable.hpp"

Eigen::Vector3f random_in_unit_sphere() {
  Eigen::Vector3f p;
  do { 
    p = 2.0*Eigen::Vector3f::Random() - Eigen::Vector3f(1,1,1); 
  } while (p.dot(p) >= 1.0); 
  return p;
} 

Eigen::Vector3f reflect(const Eigen::Vector3f& v, const Eigen::Vector3f& n) {
  return v - 2*v.dot(n)*n;
}

bool refract(const Eigen::Vector3f& v, const Eigen::Vector3f &n, const float ni_over_nt, Eigen::Vector3f &refracted)
{
  const Eigen::Vector3f uv = v/v.norm();
  const float dt  = uv.dot(n);
  const float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1.0 - dt*dt);
  if (discriminant > 0) {
    refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
    return true;
  }
  else
    return false;
}


float schlick(const float cosine, const float ref_idx)
{
  float r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 = r0*r0;
  return r0 + (1 - r0)*pow((1 - cosine), 5);
}


class material {
  public: 
    virtual bool scatter(const ray &r_in, const hit_record &rec,
        Eigen::Vector3f &attenuation, ray& scattered) const = 0;
    virtual Eigen::Vector3f emitted(const float u, const float v, const Eigen::Vector3f& p) const {
      return Eigen::Vector3f::Zero();
    }
};


class lambertian : public material {
  public:
    lambertian(texture *a) : albedo(a) {}

    virtual bool  scatter(const ray &r_in, const hit_record &rec,
        Eigen::Vector3f &attenuation, ray &scattered) const
    {

      Eigen::Vector3f target = rec.p + rec.normal + random_in_unit_sphere();
      scattered = ray(rec.p, target-rec.p, r_in.time());
      attenuation = albedo->value(rec.u, rec.v, rec.p);
      return true;
    }

    texture* albedo;
};


class metal : public material {
  public:
    metal(const Eigen::Vector3f a, const float f): albedo(a) {
      if (f < 1) 
        fuzz = f;
      else
        fuzz = 1;
    }
  
    virtual bool scatter(const ray &r_in, const hit_record &rec,
        Eigen::Vector3f &attenuation, ray &scattered) const
    {
      const Eigen::Vector3f reflected = reflect(r_in.direction()/r_in.direction().norm(), rec.normal);
      scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
      attenuation = albedo;
      return scattered.direction().dot(rec.normal) > 0;
    }

    float fuzz;
    Eigen::Vector3f albedo;
};


class dielectric : public material {
  public:
    dielectric(float ni) : ref_idx(ni) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec,
        Eigen::Vector3f &attenuation, ray &scattered) const 
    {
      Eigen::Vector3f outward_normal;
      Eigen::Vector3f reflected = reflect(r_in.direction(), rec.normal);
      float ni_over_nt;
      attenuation = Eigen::Vector3f::Ones();
      Eigen::Vector3f refracted;
      float reflect_prob;
      float cosine;
      if(r_in.direction().dot(rec.normal) > 0)
      {
        outward_normal = -rec.normal;
        ni_over_nt = ref_idx;
        cosine = ref_idx * r_in.direction().dot(rec.normal) / 
          r_in.direction().norm();
      }
      else 
      {
        outward_normal = rec.normal;
        ni_over_nt = 1.0/ref_idx;
        cosine = -r_in.direction().dot(rec.normal) / r_in.direction().norm();
      }
      if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
      {
        reflect_prob = schlick(cosine, ref_idx);
      }
      else
      {
        reflect_prob = 1.0;
      }
      if (drand48() < reflect_prob)
      {
        scattered = ray(rec.p, reflected);
      }
      else
      {
        scattered = ray(rec.p, refracted);
      }
      return true;
    }

    float ref_idx;
};

class diffuse_light : public material {
  public:
    diffuse_light(texture *a) : emit(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, 
        Eigen::Vector3f& attenuation, ray& scatter) const { 
      return false;}
    virtual Eigen::Vector3f emitted(const float u, const float v, const Eigen::Vector3f& p) const {
      return emit->value(u, v, p);
    }
    
    texture *emit;
};


class isotropic : public material {
  public: 
    isotropic(texture* a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, Eigen::Vector3f& attenuation, ray& scattered) const {
      scattered = ray(rec.p,random_in_unit_sphere());
      attenuation = albedo->value(rec.u, rec.v, rec.p);
      return true;
    }
    texture* albedo;
};

#endif
