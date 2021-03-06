#ifndef BOX_HPP
#define BOX_HPP
#include "hitable.hpp"
#include "rectangle.hpp"
#include "hitablelist.hpp"

class box: public hitable {
  public:
    box() {}
    box(const Eigen::Vector3f& p0, const Eigen::Vector3f& p1,
        std::shared_ptr<material> ptr);
    virtual bool hit(const ray& r, const float t0, const float t1, hit_record& rec) const;
    virtual bool bounding_box(const float t0, const float t1, aabb& box) const {
      box = aabb(pmin, pmax);
      return true;
    }
    Eigen::Vector3f pmin, pmax;
    hitable* list_ptr;
};

box::box(const Eigen::Vector3f& p0, const Eigen::Vector3f& p1, 
    std::shared_ptr<material> ptr) {
  pmin = p0;
  pmax = p1;
  hitable** list = new hitable*[6];
  list[0] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
  list[1] = new flip_normals(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
  list[2] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
  list[3] = new flip_normals(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
  list[4] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
  list[5] = new flip_normals(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
  list_ptr = new hitable_list(list, 6);
}

bool box::hit(const ray& r, const float t0, const float t1, hit_record& rec) const {
  return list_ptr->hit(r, t0, t1, rec);
}

#endif //BOX_HPP
