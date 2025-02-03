#pragma once
#include "../global/base.h"

/*
* class Raster
* 对外提供静态函数接口，传入离散的图元点，返回光栅化后的像素数组
*/
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
};