#pragma once
#include "dataStructures.h"

class Clipper {
public:
	static void doClipSpace(const uint32_t& drawMode, const std::vector<VsOutput>& primitives, std::vector<VsOutput>& outputs);

	static bool cullFace(
		const uint32_t& frontFace, 
		const uint32_t& cullFace, 
		const VsOutput& v0, 
		const VsOutput& v1, 
		const VsOutput& v2
	);

private:
	static void sutherlandHodgman(const uint32_t& drawMode, const std::vector<VsOutput>& primitive, std::vector<VsOutput>& outputs);

	static bool inside(const math::vec4f& point, const math::vec4f& plane);

	static VsOutput intersect(const VsOutput& last, const VsOutput& current, const math::vec4f& plane);
};