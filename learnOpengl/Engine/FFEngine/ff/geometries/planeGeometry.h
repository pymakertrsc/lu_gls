#pragma once 
#include "../core/geometry.h"

namespace ff {

	class PlaneGeometry :public Geometry {
	public:
		using Ptr = std::shared_ptr<PlaneGeometry>;
		static Ptr create(float width, float height, float widthSegments, float heightSegments) {
			return std::make_shared<PlaneGeometry>(width, height, widthSegments, heightSegments);
		}

		PlaneGeometry(float width, float height, float widthSegments, float heightSegments) noexcept;

		~PlaneGeometry() noexcept;

	public:
		float mWidth{ 0.0f };
		float mHeight{ 0.0f };
		float mWidthSegments{ 0.0f };
		float mHeightSegments{ 0.0f };
	};
}