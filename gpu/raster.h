#pragma once
#include "../global/base.h"


class Raster {
public:
	Raster();
	~Raster();

	static void rasterizeLine(
		std::vector<Point>& results,
		const Point& v0,
		const Point& v1
	);

	static void interpolantLine(const Point& v0, const Point& v1, Point& target);

	static void rasterizeTriangle(
		std::vector<Point>& results,
		const Point& v0,
		const Point& v1,
		const Point& v2
	);

	static void interpolantTriangle(const Point& v0, const Point& v1, const Point& v2, Point& p);

	static RGBA lerpRGBA(const RGBA& c0, const RGBA& c1, float weight);

	static RGBA lerpRGBA(const RGBA& c0, const RGBA& c1, const RGBA& c2, float weight0, float weight1, float weight2);

	static math::vec2f lerpUV(const math::vec2f& uv0, const math::vec2f& uv1, const math::vec2f& uv2, float weight0, float weight1, float weight2);








};