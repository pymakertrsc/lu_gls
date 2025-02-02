#include "Window.h"
#include"WindowsWindow.h"

namespace RGS {
	Window::Window(std::string title,const int width,const int height)
		:m_Title(title),m_Width(width),m_Height(height){ }

	void Window::Init() {
		WindowsWindow::Init();
	}

	void Window::Terminate() {
		WindowsWindow::Terminate();
	}

	Window* Window::Create(const std::string title, const int width, const int height) 
	{
//ifdef RGS_PLATFORM_WINDOWS
		return new WindowsWindow(title, width, height);
//#endif // RGS_PLATFORM_WINDOWS
//#ifdef RGS_PLATFORM_MAC
		//return new MacWindow(title, width, height);
//#endif
		
	}

}