#include"Application.h"
#include"Window.h"
#include<string>
#include<memory>


namespace RGS {
	Application::Application(std::string name,const int width,const int height)
		:m_Name(name),m_Width(width),m_Height(height)
	{
		//初始化顺序是按照类中声明顺序是相同的


		Init();
	}

	Application::~Application()
	{
		Terminate();
	}

	void Application::Init() 
	{
		Window::Init();
	}

	void Application::Terminate() 
	{
		Window::Terminate();
	}

	void Application::Run() {
		bool running = true;
		while (running) {
			OnUpdate();
		}

	}
	void Application::OnUpdate() {

	}







}