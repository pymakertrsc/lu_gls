#pragma once
#include "../global/base.h"

namespace ff {

	//����һ����򵥵ķ���id�ķ�ʽ����û�п���id�Ļ���������
	class Identity {
	public:
		static ID generateID() { return ++mCurrentID; }

	private:
		static ID mCurrentID;
	};
	
}