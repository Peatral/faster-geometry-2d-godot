#include "delaunator.hpp"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <vector>

#include <delaunator-cpp/include/delaunator.hpp>

void Delaunator::_bind_methods() {
  ClassDB::bind_method(D_METHOD("triangulate", "coords"), &Delaunator::triangulate);
  ClassDB::bind_method(D_METHOD("get_triangles"), &Delaunator::get_triangles);
  ClassDB::bind_method(D_METHOD("get_halfedges"), &Delaunator::get_halfedges);
  ClassDB::bind_method(D_METHOD("get_halfedges_by_endpoint"), &Delaunator::get_halfedges_by_endpoint);
  ClassDB::bind_method(D_METHOD("points_of_triangle", "triangle"), &Delaunator::points_of_triangle);
  ClassDB::bind_method(D_METHOD("triangles_adjacent_to_triangle", "triangle"), &Delaunator::triangles_adjacent_to_triangle);
  ClassDB::bind_method(D_METHOD("triangle_center", "coords", "triangle", "centroid_lerp"), &Delaunator::triangle_center);
  ClassDB::bind_method(D_METHOD("edges_around_point", "point"), &Delaunator::edges_around_point);
  ClassDB::bind_method(D_METHOD("triangles_around_point", "point"), &Delaunator::triangles_around_point);
  ClassDB::bind_method(D_METHOD("tri_count"), &Delaunator::tri_count);
}

PackedInt32Array Delaunator::edges_of_triangle(const int triangle) {
  PackedInt32Array result = PackedInt32Array();
  result.resize(3);

  result.set(0, 3 * triangle);
  result.set(1, 3 * triangle + 1);
  result.set(2, 3 * triangle + 2);

  return result;
}

int Delaunator::triangle_of_edge(const int edge) {
  return edge / 3;
}

int Delaunator::next_halfedge(const int edge) {
  if (edge % 3 == 2) {
    return edge - 2;
  }
  return edge + 1;
}

int Delaunator::prev_halfedge(const int edge) {
  if (edge % 3 == 0) {
    return edge + 2;
  }
  return edge - 1;
}

Vector2 Delaunator::centroid(const Vector2 a, const Vector2 b, const Vector2 c) {
  return (a + b + c) / 3;
}

Vector2 Delaunator::circumcenter(const Vector2 a, const Vector2 b, const Vector2 c) {
  const double ax = a.x;
  const double ay = a.y;
  const double bx = b.x;
  const double by = b.y;
  const double cx = c.x;
  const double cy = c.y;

  const double dx = bx - ax;
  const double dy = by - ay;
  const double ex = cx - ax;
  const double ey = cy - ay;

  const double bl = dx * dx + dy * dy;
  const double cl = ex * ex + ey * ey;
  const double d = dx * ey - dy * ex;

  const double x = ax + (ey * bl - dy * cl) * 0.5 / d;
  const double y = ay + (dx * cl - ex * bl) * 0.5 / d;

  return Vector2(x, y);
}


Delaunator::Delaunator() {

}

Delaunator::~Delaunator() {

}

void Delaunator::triangulate(PackedVector2Array coords) {
  std::vector<double> coords_vec = std::vector<double>();

  coords_vec.resize(2 * coords.size());
  for (int idx = 0; idx < coords.size(); ++idx) {
    coords_vec[2 * idx] = coords[idx].x;
    coords_vec[2 * idx + 1] = coords[idx].y;
  }

  delaunator::Delaunator d(coords_vec);

  triangles = PackedInt32Array();
  triangles.resize(d.triangles.size());

  for (int idx = 0; idx < d.triangles.size(); ++idx) {
    triangles.set(idx, d.triangles[idx]);
  }

  halfedges = PackedInt32Array();
  halfedges.resize(d.halfedges.size());

  for (int idx = 0; idx < d.halfedges.size(); ++idx) {
    halfedges.set(idx, d.halfedges[idx]);
  }

  halfedges_by_endpoint = PackedInt32Array();
  halfedges_by_endpoint.resize(coords.size());
  halfedges_by_endpoint.fill(-1);

  for (int edge = 0; edge < triangles.size(); ++edge) {
    int next = next_halfedge(edge);
    int endpoint = triangles[next];
    if (halfedges_by_endpoint[endpoint] == -1 || halfedges[edge] == -1) {
      halfedges_by_endpoint.set(endpoint, edge);
    }
  }
}

PackedInt32Array Delaunator::get_triangles() {
  return triangles;
}

PackedInt32Array Delaunator::get_halfedges() {
  return halfedges;
}

PackedInt32Array Delaunator::get_halfedges_by_endpoint() {
  return halfedges_by_endpoint;
}

PackedInt32Array Delaunator::points_of_triangle(const int triangle) {
  PackedInt32Array edges = edges_of_triangle(triangle);
  PackedInt32Array result = PackedInt32Array();
  result.resize(3);
  for (int idx = 0; idx < 3; ++idx) {
    result.set(idx, triangles[edges[idx]]);
  }
  return result;
}

PackedInt32Array Delaunator::triangles_adjacent_to_triangle(const int triangle) {
  PackedInt32Array edges = edges_of_triangle(triangle);
  PackedInt32Array result = PackedInt32Array();
  for (int idx = 0; idx < 3; ++idx) {
    int edge = edges[idx];
    int opposite = halfedges[edge];
    if (opposite >= 0) {
      result.append(triangle_of_edge(opposite));
    }
  }
  return result;
}

Vector2 Delaunator::triangle_center(const PackedVector2Array coords, const int triangle, const float centroid_lerp) {
  PackedInt32Array points = points_of_triangle(triangle);
  PackedVector2Array vertices = PackedVector2Array();
  vertices.resize(3);
  for (int idx = 0; idx < 3; ++idx) {
    vertices.set(idx, coords[points[idx]]);
  }
  const Vector2 circ = circumcenter(vertices[0], vertices[1], vertices[2]);
  const Vector2 cent = centroid(vertices[0], vertices[1], vertices[2]);
  return circ + (cent - circ) * centroid_lerp;
}

PackedInt32Array Delaunator::edges_around_point(const int point) {
  PackedInt32Array result = PackedInt32Array();
  int start = halfedges_by_endpoint[point];
  int incoming = start;

  result.append(incoming);
  int outgoing = next_halfedge(incoming);
  incoming = halfedges[outgoing];

  while (incoming != -1 && incoming != start) {
    result.append(incoming);
    outgoing = next_halfedge(incoming);
    incoming = halfedges[outgoing];
  }

  return result;
}

PackedInt32Array Delaunator::triangles_around_point(const int point) {
  PackedInt32Array edges = edges_around_point(point);
  PackedInt32Array result = PackedInt32Array();
  result.resize(edges.size());
  for (int idx = 0; idx < edges.size(); ++idx) {
    result.set(idx, triangle_of_edge(edges[idx]));
  }
  return result;
}

int Delaunator::tri_count() {
  return triangles.size() / 2;
}