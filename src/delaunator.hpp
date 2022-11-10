#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <vector>

using namespace godot;

class Delaunator : public RefCounted
{
	GDCLASS(Delaunator, RefCounted);

protected:
	static void _bind_methods();

private:
  PackedInt32Array triangles;
  PackedInt32Array halfedges;
  PackedInt32Array halfedges_by_endpoint;

  static PackedInt32Array edges_of_triangle(const int triangle);
  static int triangle_of_edge(const int edge);
  static int next_halfedge(const int edge);
  static int prev_halfedge(const int edge);

  static Vector2 centroid(const Vector2 a, const Vector2 b, const Vector2 c);
  static Vector2 circumcenter(const Vector2 a, const Vector2 b, const Vector2 c);

public:
	Delaunator();
	~Delaunator();

  void triangulate(PackedVector2Array coords);

  PackedInt32Array get_triangles();
  PackedInt32Array get_halfedges();
  PackedInt32Array get_halfedges_by_endpoint();

  PackedInt32Array points_of_triangle(const int triangle);
  PackedInt32Array triangles_adjacent_to_triangle(const int triangle);
  Vector2 triangle_center(const PackedVector2Array coords, const int triangle, const float centroid_lerp);
  PackedInt32Array edges_around_point(const int point);
  PackedInt32Array triangles_around_point(const int point);
  int tri_count();
};
