// FluidSim.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <stdexcept>
#include "GLViewer.h"

int main(int argc, char* argv[])
{
	try {
		GLViewer::initInstance("OpenGL test", 800, 600, argc, argv);
		GLViewer::MainLoop();

		std::cout << "After main loop call" << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Error with window: " << e.what() << std::endl;
	}

	system("pause");

	return 0;
}

