#include "poisson_disc_sampling.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <array>
#include <vector>

#include <poisson-disk-sampling/thinks/poisson_disk_sampling/poisson_disk_sampling.h>

using namespace godot;

void PoissonDiscSampling::_bind_methods()
{
	ClassDB::bind_static_method("PoissonDiscSampling", D_METHOD("calculate", "size", "radius", "max_sample_attempts", "seed"), &PoissonDiscSampling::calculate);
}

PoissonDiscSampling::PoissonDiscSampling()
{
}

PoissonDiscSampling::~PoissonDiscSampling()
{
}

PackedVector2Array PoissonDiscSampling::calculate(const Vector2 size, const real_t radius, const int max_sample_attempts, const int seed) {
  const auto kXMin = std::array<real_t, 2>{{0, 0}};
  const auto kXMax = std::array<real_t, 2>{{size.x, size.y}};

  std::vector<std::array<real_t, 2>> res = thinks::PoissonDiskSampling(radius, kXMin, kXMax, max_sample_attempts, seed);

  PackedVector2Array result = PackedVector2Array();
  result.resize(res.size());

  for (int idx = 0; idx < res.size(); ++idx) {
    result.set(idx, Vector2(res[idx][0], res[idx][1]));
  }

  return result;
}