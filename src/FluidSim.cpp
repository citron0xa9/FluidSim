// FluidSim.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <queue>
#include <functional>
#include "ui/GLViewer.h"

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
				glm::dvec3 pos;
				for (int i = 0; i < 3; i++) {
					std::cin >> pos[i];
				}
				inst->scene().camera().position(pos);
			}
			else if (cmd == "setCameraFovY") {
				float f;
				std::cin >> f;
				inst->scene().camera().fovY(f);
			}
			else if (cmd == "pauseSim") {
				//inst->vortonSim().simulating(false);
                //inst->rigidBodySim().simulating(false);
			}
			else if (cmd == "continueSim") {
				//inst->vortonSim().simulating(true);
                //inst->rigidBodySim().simulating(true);
			}
			else if (cmd == "resetSim") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.resetSim(true); });
			}
			else if (cmd == "setTimescale") {
				double f;
				std::cin >> f;
				//inst->vortonSim().simulationTimescale(f);
                //inst->rigidBodySim().simulationTimescale(f);
			}
			else if (cmd == "showTracers") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().tracersRendered(true); });
			}
			else if (cmd == "hideTracers") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().tracersRendered(false); });
			}
			else if (cmd == "showVortons") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().vortonsRendered(true); });
			}
			else if (cmd == "hideVortons") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().vortonsRendered(false); });
			}
			else if (cmd == "showVGrid") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().velocityGridRendered(true); });
			}
			else if (cmd == "hideVGrid") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().velocityGridRendered(false); });
			}
			else if (cmd == "showVV") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().velocityVectorsRendered(true); });
			}
			else if (cmd == "hideVV") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().velocityVectorsRendered(false); });
			} else if (cmd == "showVLines") {
                g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().velocityLinesRendered(true); });
            } else if (cmd == "hideVLines") {
                g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().velocityLinesRendered(false); });
            }
			else if (cmd == "VVRes") {
				float f;
				std::cin >> f;
				g_GlDpenedentCommands.push([f](GLViewer &viewer) {(viewer.vortonSimRenderer().VelocityVectorsRendererPtr())->gridResolutionFactor(f); });
			}
			else if (cmd == "showOct") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().vortonOctHeapRendered(true); });
			}
			else if (cmd == "hideOct") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().vortonOctHeapRendered(false); });
			}
			else if (cmd == "OctLevel") {
				size_t l;
				std::cin >> l;
				g_GlDpenedentCommands.push([l](GLViewer &viewer) {(viewer.vortonSimRenderer().vortonOctHeapRendererPtr())->renderedLevel(l); });
			}
			else if (cmd == "showOctV") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().vortonsOctHeapRendered(true); });
			}
			else if (cmd == "hideOctV") {
				g_GlDpenedentCommands.push([](GLViewer &viewer) {viewer.vortonSimRenderer().vortonsOctHeapRendered(false); });
			}
			else if (cmd == "OctLevelV") {
				size_t l;
				std::cin >> l;
				g_GlDpenedentCommands.push([l](GLViewer &viewer) {(viewer.vortonSimRenderer().vortonRendererOctHeapPtr())->renderedLevel(l); });
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

