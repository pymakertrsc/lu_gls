#pragma once
#include "../interpolant.h"

namespace ff {
	class LinearInterpolant :public Interpolant {
	public:
		using Ptr = std::shared_ptr<LinearInterpolant>;
		static Ptr create(
			const std::vector<float>& parameterPositions,
			const std::vector<float>& sampleValues,
			const uint32_t& sampleSize,
			void* resultBuffer = nullptr) {
			return std::make_shared<LinearInterpolant>(parameterPositions, sampleValues, sampleSize, resultBuffer);
		}
		
		LinearInterpolant(
			const std::vector<float>& parameterPositions,
			const std::vector<float>& sampleValues,
			const uint32_t& sampleSize,
			void* resultBuffer = nullptr) noexcept;

		~LinearInterpolant() noexcept;

	private:
		void interpolateInternal(
			const uint32_t& lastIndex,
			const float& lastPosition,
			const float& nextPosition,
			const float& t) override;
	};
}