#pragma once 
#include "../global/base.h"

namespace ff {

	using namespace std::chrono;
	class Timer {
	public:
		using Ptr = std::shared_ptr<Timer>;
		static Ptr create() { return std::make_shared<Timer>(); }

		Timer():mBegin(high_resolution_clock::now()) {};

		void reset() { mBegin = high_resolution_clock::now(); }

		template<typename Duration = milliseconds>
		int64_t elapsed() const {
			return duration_cast<Duration>(high_resolution_clock::now() - mBegin).count();
		}

		int64_t elapsed_mill() const {
			return elapsed<milliseconds>();
		}

		int64_t elapsed_micro() const {
			return elapsed<microseconds>();
		}

		int64_t elapsed_nano() const {
			return elapsed<nanoseconds>();
		}

	private:
		time_point<high_resolution_clock> mBegin;
	};
}