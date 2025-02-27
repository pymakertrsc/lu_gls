#pragma once 
#include "planeGeometry.h"

namespace ff {

	PlaneGeometry::PlaneGeometry(float width, float height, float widthSegments, float heightSegments) noexcept:Geometry() {
		mWidth = width;
		mHeight = height;
		mWidthSegments = widthSegments;
		mHeightSegments = heightSegments;

		float widthHalf = width / 2.0f;
		float heightHalf = height / 2.0f;
	
		float gridX = std::floor(widthSegments);
		float gridY = std::floor(heightSegments);

		float gridX1 = gridX + 1;
		float gridY1 = gridY + 1;

		float segmentWidth = width / gridX;
		float segmentHeight = height / gridX;

		std::vector<uint32_t> indices{};
		std::vector<float> positions{};
		std::vector<float> normals{};
		std::vector<float> uvs{};

		for (int iy = 0; iy < gridY1; iy++) {
			float y = iy * segmentHeight - heightHalf;

			for (int ix = 0; ix < gridX1; ix++) {
				float x = ix * segmentWidth - widthHalf;
				positions.push_back(x);
				positions.push_back(-y);
				positions.push_back(0);

				normals.push_back(0);
				normals.push_back(0);
				normals.push_back(1);

				uvs.push_back(ix / gridX);
				uvs.push_back(1.0 - iy / gridY);
			}
		}

		for (int iy = 0; iy < gridY; iy++) {

			for (int ix = 0; ix < gridX; ix++) {

				uint32_t a = ix + gridX1 * iy;
				uint32_t b = ix + gridX1 * (iy + 1);
				uint32_t c = (ix + 1) + gridX1 * (iy + 1);
				uint32_t d = (ix + 1) + gridX1 * iy;

				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(d);

				indices.push_back(b);
				indices.push_back(c);
				indices.push_back(d);
			}
		}

		setIndex(Attributei::create(indices, 1));
		setAttribute("position", Attributef::create(positions, 3));
		setAttribute("normal", Attributef::create(normals, 3));
		setAttribute("uv", Attributef::create(uvs, 2));
	}

	PlaneGeometry::~PlaneGeometry() noexcept {}

}
