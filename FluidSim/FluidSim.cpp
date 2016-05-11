// FluidSim.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <queue>
#include <functional>
#include "GLViewer.h"

std::queue<std::function<void(GLViewer &viewer)>> g_GlDpenedentCommands;

void cli() {
	std::this_thread::sleep_for(std::chrono::seconds(3));
	while (true) {
		std::cout << "Enter command: " << std::endl;
		try {
			std::string cmd;
			std::cin >> cmd;
			GLViewer *inst = GLViewer::instance();
			if (cmd == "exit") {
				break;
			}
			if (inst == nullptr) {
				throw std::runtime_error("Command entered but no instance of glviewer available");
			}
			if (cmd == "setCameraPos") {
				glm::vec3 pos;
				for (int i = 0; i < 3; i++) {
					std::cin >> pos[i];
				}
				//inst->getScene().getCamera().setPosition(pos);
			}
			else if (cmd == "setCameraFovY") {
				float f;
				std::cin >> f;
				inst->scene().camera().fovY(f);
			}
			else if (cmd == "pauseSim") {
				inst->vortonSim().simulating(false);
			}
			else if (cmd == "continueSim") {
				inst->vortonSim().simulating(true);
			}
			else if (cmd == "resetSim") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.resetSim(true); });
			}
			else if (cmd == "setTimescale") {
				float f;
				std::cin >> f;
				inst->vortonSim().simulationTimescale(f);
			}
			else {
				throw std::runtime_error("Unknown cli command");
			}
		}
		catch (const std::runtime_error &e) {
			std::cout << "Error in cli: " << e.what() << std::endl;
		}
	}
	std::cout << "Exited cli" << std::endl;
}

int main(int argc, char* argv[])
{
	std::thread cliT(cli);
	try {
		GLViewer::initInstance("OpenGL test", 800, 600);
		GLViewer *viewer = GLViewer::instance();
		while (!viewer->shouldClose()) {
			viewer->cycle();
			while (!g_GlDpenedentCommands.empty()) {
				(g_GlDpenedentCommands.front())(*viewer);
				g_GlDpenedentCommands.pop();
			}
		}
		GLViewer::deleteInstance();
	}
	catch (const std::exception& e) {
		std::cerr << "Error with window: " << e.what() << std::endl;
	}
	
	
	std::cout << "After main loop" << std::endl;
	std::cout << "waiting for cli to exit..." << std::endl;
	cliT.join();
	std::cout << "cli finished, exiting..." << std::endl;


	system("pause");

	return 0;
}

