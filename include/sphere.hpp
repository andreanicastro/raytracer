#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "hitable.hpp"
#include "material.hpp"

class sphere: public hitable {
  public: 
    sphere() {}
    sphere(vec3 cen, float r, material* mat) : center(cen), radius(r) {
      mat_ptr = mat;
    };
    virtual bool hit(const ray& r, const float t_min, const float t_max, hit_record& rec) const;
    virtual bool bounding_box(const float t0, const float t1, aabb& box) const;
    void get_sphere_uv(const vec3& p, float& u, float& v) const;
    vec3 center;
    float radius;
    material* mat_ptr;
};

bool sphere::hit(const ray& r, const float t_min, const float t_max, hit_record& rec) const {
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc,oc) - radius*radius;
  float discriminant = b*b - a*c;
  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant))/a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
      get_sphere_uv((rec.p - center)/radius, rec.u, rec.v);
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-b + sqrt(discriminant))/a;
    if(temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
} 

bool sphere::bounding_box(const float t0, const float t1, aabb& box) const {
  box = aabb(center - vec3(radius, radius, radius),
             center + vec3(radius, radius, radius));
  return true;
}

class moving_sphere : public hitable {
  public:
    moving_sphere() {}
    moving_sphere(vec3 cen0, vec3 cen1, const float t0, const float t1, float r, 
        material *m) :
      center0(cen0), center1(cen1), 
      time0(t0), time1(t1), 
      radius(r), mat_ptr(m) {};
    virtual bool hit(const ray& r, 
        const float t_min, const float t_max, hit_record& rec) const;
    virtual bool bounding_box(const float t0, const float t1, aabb& box) const;

    vec3 center(const float time) const;
    vec3 center0, center1;
    float time0, time1;
    float radius;
    material *mat_ptr;
};

vec3 moving_sphere::center(const float time) const {
  return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray& r, const float t_min, const float t_max, 
    hit_record& rec) const {
  vec3 oc = r.origin() - center(r.time());
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius*radius;
  float discriminant = b*b - a*c;
  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant))/a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center(r.time()))/radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-b + sqrt(discriminant))/a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center(r.time()))/radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

bool moving_sphere::bounding_box(const float t0, const float t1, aabb& box) const {
  aabb boxt0 = aabb(center(t0) - vec3(radius, radius, radius),
                    center(t0) + vec3(radius, radius, radius));
  aabb boxt1 = aabb(center(t1) - vec3(radius, radius, radius),
                    center(t1) + vec3(radius, radius, radius));
  box = surrounding_box(boxt0, boxt1);
  return true;
}

void sphere::get_sphere_uv(const vec3& p, float& u, float& v) const{
  float phi = atan2(p.z(), p.x());
  float theta = asin(p.y());
  u = 1 - (phi + M_PI) / (2 * M_PI);
  v = (theta + M_PI/2.0) / M_PI;
}

#endif




