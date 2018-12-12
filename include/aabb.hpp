#ifndef AABB_HPP
#define AABB_HPP

#include "ray.hpp"

inline float ffmin(float a, float b) {return a < b ? a : b;}
inline float ffmax(float a, float b) {return a > b ? a : b;}

class aabb {
  public: 
    aabb(){}
    aabb(const Eigen::Vector3f& a, const Eigen::Vector3f& b) { _min = a; _max = b;}

    Eigen::Vector3f min() const {return _min; }
    Eigen::Vector3f max() const {return _max; }

    //bool hit(const ray& r, float tmin, float tmax) const {
    //  for (int a = 0; a < 3; ++a) {
    //    float t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a],
    //                     (_max[a] - r.origin()[a]) / r.direction()[a]);
    //    float t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a],
    //                     (_max[a] - r.origin()[a]) / r.direction()[a]);
    //    tmin = ffmax(t0, tmin);
    //    tmax = ffmax(t1, tmax);
    //    if (tmax <= tmin) 
    //      return false;
    //  }
    //  return true;
    //}
    
    inline bool hit(const ray& r, float tmin, float tmax) const {
      for (int a = 0; a < 3; ++a) {
        float invD = 1.0f / r.direction()[a];
        float t0 = ffmin((_min[a] - r.origin()[a]) * invD,
                         (_max[a] - r.origin()[a]) * invD);

        float t1 = ffmax((_min[a] - r.origin()[a]) * invD,
                         (_max[a] - r.origin()[a]) * invD);
        //if(invD < 0.0f)
        //  std::swap(t0, t1);
        tmin = ffmax(t0, tmin);
        tmax = ffmin(t1, tmax);
        if (tmax <= tmin) 
          return false;
      }
      return true;
    }


    Eigen::Vector3f _min;
    Eigen::Vector3f _max;
};

aabb surrounding_box(aabb box0, aabb box1) {
  Eigen::Vector3f small(ffmin(box0.min().x(), box1.min().x()),
             ffmax(box0.min().y(), box1.min().y()),
             ffmin(box0.min().z(), box1.min().z()));
  Eigen::Vector3f big  (ffmin(box0.max().x(), box1.max().x()),
             ffmin(box0.max().y(), box1.max().y()),
             ffmin(box0.max().z(), box1.max().z()));
  return aabb(small, big);
}

#endif //ABBBB_HPP
