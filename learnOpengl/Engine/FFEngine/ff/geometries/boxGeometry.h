#pragma once 
#include "../core/geometry.h"

namespace ff {

	class BoxGeometry :public Geometry {
	public:
		static constexpr uint32_t U = 0;
		static constexpr uint32_t V = 1;
		static constexpr uint32_t W = 2;

		using PositionArray = std::vector<float>;
		using NormalArray = std::vector<float>;
		using UVArray = std::vector<float>;
		using IndexArray = std::vector<uint32_t>;

		using Ptr = std::shared_ptr<BoxGeometry>;
		static Ptr create(float width, float height, float depth, uint32_t widthSegment = 1, uint32_t heightSegment = 1, uint32_t depthSegment = 1) {
			return std::make_shared<BoxGeometry>(width, height, depth, widthSegment, heightSegment, depthSegment);
		}
		
		BoxGeometry(float width, float height, float depth, uint32_t widthSegment, uint32_t heightSegment, uint32_t depthSegment) noexcept;

		~BoxGeometry() noexcept;

	private:
		void buildPlane(
			uint32_t u, uint32_t v, uint32_t w, 
			int16_t udir, int16_t vdir, 
			float width, float height, float depth, 
			uint32_t gradX, uint32_t gradY,
			uint32_t& numberOfVertices,
			PositionArray& positions,
			NormalArray& normals,
			UVArray& uvs,
			IndexArray& indices
		) noexcept;

	private:
		float mWidth{ 0.0f };
		float mHeight{ 0.0f };
		float mDepth{ 0.0f };
		uint32_t mWidthSegments{ 0 };
		uint32_t mHeightSegments{ 0 };
		uint32_t mDepthSegments{ 0 };
	};
}