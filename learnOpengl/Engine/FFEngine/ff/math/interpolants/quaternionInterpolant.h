#pragma once
#include "../interpolant.h"

namespace ff {
	class QuaternionInterpolant :public Interpolant {
	public:
		using Ptr = std::shared_ptr<QuaternionInterpolant>;
		static Ptr create(
			const std::vector<float>& parameterPositions,
			const std::vector<float>& sampleValues,
			const uint32_t& sampleSize,
			void* resultBuffer = nullptr) {
			return std::make_shared<QuaternionInterpolant>(parameterPositions, sampleValues, sampleSize, resultBuffer);
		}

		QuaternionInterpolant(
			const std::vector<float>& parameterPositions,
			const std::vector<float>& sampleValues,
			const uint32_t& sampleSize,
			void* resultBuffer = nullptr) noexcept;

		~QuaternionInterpolant() noexcept;

	private:
		void interpolateInternal(
			const uint32_t& lastIndex,
			const float& lastPosition,
			const float& nextPosition,
			const float& t) override;
	};
}