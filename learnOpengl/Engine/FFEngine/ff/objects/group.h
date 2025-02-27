#include "../core/object3D.h"

namespace ff {

	//һ����/һ���ڵ�,���Խ��ܶ�mesh����group���ӽڵ��£�ͳһ�����ƶ�����ת�����ŵȲ�����
	//groupOrder����ָ����Ⱦ��˳��
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