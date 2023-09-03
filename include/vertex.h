#ifndef SOFTRENDERER_INCLUDE_VERTEX_H_
#define SOFTRENDERER_INCLUDE_VERTEX_H_

#include "vector.h"

// Input to the vertex shader
struct VertexIn {
  VertexIn() = default;
  VertexIn(const Vector4d &p, const Vector4d &c, const Vector4d &n,
		   const Vector2d &t)
	  : local_position(p), color(c), normal(n), texcoord(t) {}
  VertexIn(const VertexIn &rhs)
	  : local_position(rhs.local_position),
		color(rhs.color),
		normal(rhs.normal),
		texcoord(rhs.texcoord) {}

  Vector4d local_position;
  Vector4d color;
  Vector4d normal;
  Vector2d texcoord;
};

// Output from the vertex shader
struct VertexOut {
  VertexOut() = default;
  VertexOut(const Vector4d &wp, const Vector4d &vp, const Vector4d &cp,
			const Vector4d &pp, const Vector4d &c, const Vector4d &n,
			const Vector2d &t, double odz)
	  : world_position(wp),
		view_position(vp),
		clip_position(cp),
		pixel_position(pp),
		color(c),
		normal(n),
		texcoord(t),
		one_div_z(odz) {}
  VertexOut(const VertexOut &rhs)
	  : world_position(rhs.world_position),
		view_position(rhs.view_position),
		clip_position(rhs.clip_position),
		pixel_position(rhs.pixel_position),
		color(rhs.color),
		normal(rhs.normal),
		texcoord(rhs.texcoord),
		one_div_z(rhs.one_div_z) {}

  static VertexOut Lerp(const VertexOut &v1, const VertexOut &v2, double t) {
	VertexOut res;

	res.world_position = v1.world_position + t * (v2.world_position - v1.world_position);
	res.view_position = v1.view_position + t * (v2.view_position - v1.view_position);
	res.clip_position = v1.clip_position + t * (v2.clip_position - v1.clip_position);
	// pixel_position do not lerp here
	res.color = v1.color + t * (v2.color - v1.color);
	res.normal = v1.normal + t * (v2.normal - v1.normal);
	res.texcoord = v1.texcoord + t * (v2.texcoord - v1.texcoord);
	res.one_div_z = v1.one_div_z + t * (v2.one_div_z - v1.one_div_z);

	return res;
  }

  Vector4d world_position;
  Vector4d view_position;
  Vector4d clip_position;
  Vector4d pixel_position;
  Vector4d color;
  Vector4d normal;
  Vector2d texcoord;
  double one_div_z;
};

#endif //SOFTRENDERER_INCLUDE_VERTEX_H_
