#ifndef PERLIN_HPP
#define PERLIN_HPP

#include <Eigen/Core>

inline float trilinear_interp(const float c[2][2][2], const float u, const float v, const float w) {
  float accum = 0;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < 2; ++k) {
        accum += (i * u + (1 - i) * (1 - u)) *
                 (j * v + (1 - j) * (1 - v)) * 
                 (k * w + (1 - k) * (1 - w)) * c[i][j][k];
      }
    }
  }
  return accum;
}

inline float perlin_interp(const Eigen::Vector3f c[2][2][2], const float u, const float v, const float w) {
  const float uu = u * u * (3 - 2 * u);
  const float vv = v * v * (3 - 2 * v);
  const float ww = w * w * (3 - 2 * w);
  float accum = 0;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < 2; ++k) {
        Eigen::Vector3f weight_v(u-i, v-j, w-k);
        accum += (i * uu + (1 - i) * (1 - uu)) *
                 (j * vv + (1 - j) * (1 - vv)) *
                 (k * ww + (1 - k) * (1 - ww)) *
                 c[i][j][k].dot(weight_v);
      }
    }
  }
  return accum;
}

class perlin {
  public: 
    float noise(const Eigen::Vector3f& p) const {
      const float u = p[0] - floor(p[0]);
      const float v = p[1] - floor(p[1]);
      const float w = p[2] - floor(p[2]);
      const int i = floor(p[0]);
      const int j = floor(p[1]);
      const int k = floor(p[2]);
      Eigen::Vector3f c[2][2][2];
      for (int di = 0; di < 2; ++di) {
        for (int dj = 0; dj < 2; ++dj) {
          for (int dk = 0; dk < 2; ++dk) {
            c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^
                                   perm_y[(j + dj) & 255] ^
                                   perm_z[(k + dk) & 255]];
          }
        }
      }
      return perlin_interp(c, u, v, w);
    }

    float turb(const Eigen::Vector3f& p, int depth=7) const {
      float accum = 0;
      Eigen::Vector3f temp_p = p;
      float weight = 1.0;
      for (int i = 0; i < depth; ++i) {
        accum += weight * noise(temp_p);
        weight *= 0.5;
        temp_p *= 2;
      }
      return fabs(accum);
    }

    static Eigen::Vector3f* ranvec;
    static int* perm_x;
    static int* perm_y;
    static int* perm_z;
};

static Eigen::Vector3f* perlin_generate() {
  Eigen::Vector3f* p = new Eigen::Vector3f[256];
  for (int i = 0; i < 256; ++i) {
    const Eigen::Vector3f unit_vec = -Eigen::Vector3f::Ones() + 2 * Eigen::Vector3f::Random();
    p[i] = unit_vec;
  }
  return p;
}

void permute(int *p, const int n) {
  for (int i = n - 1; i > 0; --i) {
    int target = int(drand48() * (i + 1));
    int tmp = p[i];
    p[i] = p[target];
    p[target] = tmp;
  }
  return;
}

static int* perlin_generate_perm() {
  int* p = new int[256];
  for (int i = 0; i < 256; ++i) {
    p[i] = i;
  }
  permute(p, 256);
  return p;
}

Eigen::Vector3f *perlin::ranvec = perlin_generate();
int *perlin::perm_x = perlin_generate_perm();
int *perlin::perm_y = perlin_generate_perm();
int *perlin::perm_z = perlin_generate_perm();

#endif //PERLIN_HPP
