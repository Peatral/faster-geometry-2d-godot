#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

class PoissonDiscSampling : public RefCounted {
	GDCLASS(PoissonDiscSampling, RefCounted);

protected:
  static void _bind_methods();

public:
  PoissonDiscSampling();
  ~PoissonDiscSampling();

  static PackedVector2Array calculate(const Vector2 size, const real_t radius, const int max_sample_attempts, const int seed);
};
