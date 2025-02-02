#include<iostream>
#include"Application.h"

int main() {
	std::cout << "Hello RGS!" << std::endl;


	RGS::Application app("RGS",400,300);

	app.Run();
}