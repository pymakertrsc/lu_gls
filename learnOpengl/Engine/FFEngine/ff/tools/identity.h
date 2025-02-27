#pragma once
#include "../global/base.h"

namespace ff {

	//这是一个最简单的分配id的方式，并没有考虑id的回收再利用
	class Identity {
	public:
		static ID generateID() { return ++mCurrentID; }

	private:
		static ID mCurrentID;
	};
	
}