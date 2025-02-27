#include "boxGeometry.h"

namespace ff {

	BoxGeometry::BoxGeometry(float width, float height, float depth, uint32_t widthSegments, uint32_t heightSegments, uint32_t depthSegments) noexcept:Geometry(){
		mWidth = width;
		mHeight = height;
		mDepth = depth;
		mWidthSegments = widthSegments;
		mHeightSegments = heightSegments;
		mDepthSegments = depthSegments;

		std::vector<uint32_t> indices{};
		std::vector<float> positions{};
		std::vector<float> normals{};
		std::vector<float> uvs{};

		uint32_t numberOfVertices = 0;

		buildPlane(W, V, U, -1, -1, mDepth, mHeight, mWidth, mDepthSegments, mHeightSegments, numberOfVertices, positions, normals, uvs, indices);
		buildPlane(W, V, U, 1, -1, mDepth, mHeight, -mWidth, mDepthSegments, mHeightSegments, numberOfVertices, positions, normals, uvs, indices);
		buildPlane(U, W, V, 1, 1, mWidth, mDepth, mHeight, mWidthSegments, mDepthSegments, numberOfVertices, positions, normals, uvs, indices);
		buildPlane(U, W, V, 1, -1, mWidth, mDepth, -mHeight, mWidthSegments, mDepthSegments, numberOfVertices, positions, normals, uvs, indices);
		buildPlane(U, V, W, 1, -1, mWidth, mHeight, mDepth, mWidthSegments, mHeightSegments, numberOfVertices, positions, normals, uvs, indices);
		buildPlane(U, V, W, -1, -1, mWidth, mHeight, -mDepth, mWidthSegments, mHeightSegments, numberOfVertices, positions, normals, uvs, indices);

		setAttribute("position", Attributef::create(positions, 3));
		setAttribute("normal", Attributef::create(normals, 3));
		setAttribute("uv", Attributef::create(uvs, 2));

		setIndex(Attributei::create(indices, 1));
	}

	BoxGeometry::~BoxGeometry() noexcept {}

	void BoxGeometry::buildPlane(
		uint32_t u, uint32_t v, uint32_t w,
		int16_t udir, int16_t vdir,
		float width, float height, float depth,
		uint32_t gradX, uint32_t gradY,
		uint32_t& numberOfVertices,
		PositionArray& positions,
		NormalArray& normals,
		UVArray& uvs,
		IndexArray& indices
	) noexcept {
		const float segmentWidth = width / (float)gradX;
		const float segmentHeight = height / (float)gradY;

		const float widthHalf = width / 2.0f;
		const float heightHalf = height / 2.0f;
		const float depthHalf = depth / 2.0f;

		const uint32_t gradX1 = gradX + 1;
		const uint32_t gradY1 = gradY + 1;

		uint32_t vertexCounter = 0;

		float vector[3];

		//create vertices
		float x = 0;
		float y = 0;

		// ._._._.number of vertices always bigger than segments by one
		//up-down brush
		for (uint32_t iy = 0; iy < gradY1; iy++) {
			y = iy * segmentHeight - heightHalf;

			for (uint32_t ix = 0; ix < gradX1; ++ix) {
				//从0开始看，然后左移widthhalf
				x = ix * segmentWidth - widthHalf;

				//positions
				vector[u] = x * udir;
				vector[v] = y * vdir;
				vector[w] = depthHalf;

				positions.push_back(vector[0]);
				positions.push_back(vector[1]);
				positions.push_back(vector[2]);

				//normals
				vector[u] = 0;
				vector[v] = 0;
				vector[w] = depth > 0 ? 1 : -1;

				normals.push_back(vector[0]);
				normals.push_back(vector[1]);
				normals.push_back(vector[2]);

				//uv
				uvs.push_back((float)ix / (float)gradX);
				uvs.push_back((float)iy / (float)gradY);

				//counter
				vertexCounter++;
			}
		}

		//make indices
		for (uint32_t iy = 0; iy < gradY; iy++) {
			for (uint32_t ix = 0; ix < gradX; ++ix) {
				/*
				* a---d
				* |   |
				* b---c
				*/
				uint32_t a = numberOfVertices + ix + gradX1 * iy;
				uint32_t b = numberOfVertices + ix + gradX1 * (iy + 1);
				uint32_t c = numberOfVertices + (ix + 1) + gradX1 * (iy + 1);
				uint32_t d = numberOfVertices + (ix + 1) + gradX1 * iy;

				//make faces
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(d);

				indices.push_back(b);
				indices.push_back(c);
				indices.push_back(d);
			}
		}

		numberOfVertices += vertexCounter;
	}
}