#ifndef VOLUME_HPP
#define VOLUME_HPP

#include "hitable.hpp" 
#include "texture.hpp"

#include <limits>
#include <cmath>
#include <iostream>

class constant_medium : public hitable {
  public:
    constant_medium(hitable* b, float d, std::shared_ptr<texture> a) : 
      boundary(b), density(d), phase_function(isotropic::ptr(a)) {}

    virtual bool hit(const ray& r, const float t_min,
                     const float t_max, hit_record& rec) const;

    virtual bool bounding_box(const float t0, const float t1, aabb& box) 
      const {
      return boundary->bounding_box(t0, t1, box);
    }

    hitable* boundary;
    float density;
    std::shared_ptr<material> phase_function;
};

bool constant_medium::hit(const ray& r, const float t_min, const float t_max, 
                          hit_record& rec) const {
  bool db = (drand48() < 0.00001);
  db = false;
  hit_record rec1, rec2;
  if (boundary->hit(r, std::numeric_limits<float>::min(), 
        std::numeric_limits<float>::max(), rec1)) {
    if (boundary->hit(r, rec1.t+0.0001, std::numeric_limits<float>::max(), rec2)) {
      if (db) {
        std::cerr << "\nt0 t1 "  << rec1.t << " " << rec2.t << "\n";
      }
      if (rec1.t < t_min) {
        rec1.t = t_min;
      }
      if (rec2.t > t_max) {
        rec2.t = t_max;
      }
      if (rec1.t >= rec2.t) {
        return false;
      } if (rec1.t < 0) {
        rec1.t = 0;
      }
      float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().norm();
      float hit_distance = -(1.f / density) * std::log(drand48());
      if (hit_distance < distance_inside_boundary) {
        if (db) {
          std::cerr << "hit_distance = " << hit_distance << "\n";
        }
        rec.t = rec1.t + hit_distance / r.direction().norm();
        if (db) {
          std::cerr << "rec.t = " << rec.t << "\n";
        }
        rec.p = r.point_at_parameter(rec.t);
        if (db) {
          std::cerr << "rec.p = " << rec.p << "\n";
        }
        rec.normal = Eigen::Vector3f(1, 0, 0); // Arbitrary;
        rec.mat_ptr = phase_function;
        return true;
      }
    }
  }
  return false;
}
        

#endif //VOLUME
