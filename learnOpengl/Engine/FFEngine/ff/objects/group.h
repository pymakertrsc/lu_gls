#include "../core/object3D.h"

namespace ff {

	//一个组/一个节点,可以将很多mesh挂在group的子节点下，统一进行移动、旋转、缩放等操作，
	//groupOrder可以指定渲染的顺序
	class Group :public Object3D {
	public:
		using Ptr = std::shared_ptr<Group>;
		static Ptr create() {
			return std::make_shared <Group>();
		}

		Group() noexcept {
			mIsGroup = true;
		}
		~Group() noexcept {}

		//bigger first
		uint32_t mGroupOrder{ 0 };
	};
}