#pragma once
#include<string>
#include<Windows.h>
#include "Window.h"

namespace RGS {
	class WindowsWindow : public Window 
	{
	public :
		WindowsWindow(const std::string title, const int width, const int height);
		~WindowsWindow();

		virtual void Show() const override;//就算不写virtual也是隐式的虚方法

	public :
		static void Init();
		static void Terminate();

	private:
		static void Register();
		static void Unregister();

		//static 
	};
}