#pragma once
#include "../global/base.h"
#include "dataStructures.h"

/*
* class Raster
* 对外提供静态函数接口，传入离散的图元点，返回光栅化后的像素数组
*/
class Raster {
public:
	Raster();
	~Raster();

	static void rasterize(
		std::vector<VsOutput>& results,
		const uint32_t& drawMode,
		const std::vector<VsOutput>& inputs
	);

private:
	static void rasterizeLine(
		std::vector<VsOutput>& results,
		const VsOutput& v0,
		const VsOutput& v1
	);

	static void interpolantLine(const VsOutput& v0, const VsOutput& v1, VsOutput& target);

	static void rasterizeTriangle(
		std::vector<VsOutput>& results,
		const VsOutput& v0,
		const VsOutput& v1,
		const VsOutput& v2
	);

	static void interpolantTriangle(const VsOutput& v0, const VsOutput& v1, const VsOutput& v2, VsOutput& p);

};