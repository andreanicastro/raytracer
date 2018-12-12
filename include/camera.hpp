#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "ray.hpp"

Eigen::Vector3f random_in_unit_disk() {
    Eigen::Vector3f p;
    do {
        p = 2.0*Eigen::Vector3f(drand48(),drand48(),0) - Eigen::Vector3f(1,1,0);
    } while (p.squaredNorm() >= 1.0);
    return p;
}

class camera {
    public:
        camera(const Eigen::Vector3f lookfrom, const Eigen::Vector3f lookat, const Eigen::Vector3f vup, const float vfov, 
            const float aspect, const float aperture, const float focus_dist,
            const float t0, const float t1) :
          time0(t0), time1(t1), lens_radius(aperture/2) 
        { // vfov is top to bottom in degrees
          const float theta = vfov*M_PI/180;
          const float half_height = tan(theta/2);
          const float half_width = aspect * half_height;
          origin = lookfrom;
          w = (lookfrom - lookat).normalized();
          u = (vup.cross(w)).normalized();
          v = w.cross(u);
          lower_left_corner = origin  - half_width*focus_dist*u -
            half_height*focus_dist*v - focus_dist*w;
          horizontal = 2*half_width*focus_dist*u;
          vertical = 2*half_height*focus_dist*v;
        }

        ray get_ray(const float s, const float t) {
            Eigen::Vector3f rd = lens_radius*random_in_unit_disk();
            Eigen::Vector3f offset = u * rd.x() + v * rd.y();
            float time = time0 + drand48()*(time1 - time0);
            return ray(origin + offset, lower_left_corner + s*horizontal + 
                t*vertical - origin - offset, time); 
        }

        Eigen::Vector3f origin;
        Eigen::Vector3f lower_left_corner;
        Eigen::Vector3f horizontal;
        Eigen::Vector3f vertical;
        Eigen::Vector3f u, v, w;
        float time0, time1;
        float lens_radius;
};
#endif


