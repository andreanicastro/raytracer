#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "hitable.hpp"
#include "material.hpp"

class sphere: public hitable {
  public: 
    sphere() {}
    sphere(Eigen::Vector3f cen, float r, std::shared_ptr<material> mat) : 
      center(cen), radius(r), mat_ptr(mat) {};

    virtual bool hit(const ray& r, const float t_min, const float t_max, hit_record& rec) const;
    virtual bool bounding_box(const float t0, const float t1, aabb& box) const;
    void get_sphere_uv(const Eigen::Vector3f& p, float& u, float& v) const;
    Eigen::Vector3f center;
    float radius;
    std::shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, const float t_min, const float t_max, hit_record& rec) const {
  Eigen::Vector3f oc = r.origin() - center;
  float a = r.direction().dot(r.direction());
  float b = oc.dot(r.direction());
  float c = oc.dot(oc) - radius*radius;
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
  box = aabb(center - Eigen::Vector3f(radius, radius, radius),
             center + Eigen::Vector3f(radius, radius, radius));
  return true;
}

class moving_sphere : public hitable {
  public:
    moving_sphere() {}
    moving_sphere(Eigen::Vector3f cen0, Eigen::Vector3f cen1, const float t0, const float t1, float r, 
        std::shared_ptr<material> m) :
      center0(cen0), center1(cen1), 
      time0(t0), time1(t1), 
      radius(r), mat_ptr(m) {};

    virtual bool hit(const ray& r, 
        const float t_min, const float t_max, hit_record& rec) const;
    virtual bool bounding_box(const float t0, const float t1, aabb& box) const;

    Eigen::Vector3f center(const float time) const;
    Eigen::Vector3f center0, center1;
    float time0, time1;
    float radius;
    std::shared_ptr<material> mat_ptr;
};

Eigen::Vector3f moving_sphere::center(const float time) const {
  return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray& r, const float t_min, const float t_max, 
    hit_record& rec) const {
  Eigen::Vector3f oc = r.origin() - center(r.time());
  float a = r.direction().dot(r.direction());
  float b = oc.dot(r.direction());
  float c = oc.dot(oc) - radius*radius;
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
  aabb boxt0 = aabb(center(t0) - Eigen::Vector3f(radius, radius, radius),
                    center(t0) + Eigen::Vector3f(radius, radius, radius));
  aabb boxt1 = aabb(center(t1) - Eigen::Vector3f(radius, radius, radius),
                    center(t1) + Eigen::Vector3f(radius, radius, radius));
  box = surrounding_box(boxt0, boxt1);
  return true;
}

void sphere::get_sphere_uv(const Eigen::Vector3f& p, float& u, float& v) const{
  float phi = atan2(p.z(), p.x());
  float theta = asin(p.y());
  u = 1 - (phi + M_PI) / (2 * M_PI);
  v = (theta + M_PI/2.0) / M_PI;
}

#endif




