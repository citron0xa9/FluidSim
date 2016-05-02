#pragma once

#include "ContainerObject.h"
#include "ActiveObject.h"

#include <vector>
#include <memory>
#include <glm/vec3.hpp>
#include "VorticityDistribution.h"
#include "VortonOctHeapElement.h"
#include "VortonOctHeap.h"
#include "UniformGrid.h"
#include "Vorton.h"

class VortonSim : public ContainerObject, public ActiveObject, public DrawableObject
{
public:
	
	VortonSim(ContainerObject &container, float viscosity, float density, const VorticityDistribution &initialVorticity, float vorticityMagnitude, const TriangleNetObject &vortonPrototype);
	VortonSim(const VortonSim &original);
	~VortonSim();

	virtual void step(float secondsPassed) override;
	virtual void render(const glm::mat4x4 &viewProjectTransform) override;
	virtual void registerContainerObjectHooks() override;

	void update(float seconds);

	void vortonsRendered(bool areRendered);
	void tracersRendered(bool areRendered);
	void simulating(bool isSimulating);

	virtual Object* copy() const override;
	
	const VortonSim& operator=(const VortonSim& original) = delete;

private:

	void initializeVortons(const VorticityDistribution &initialVorticity, float vorticityMagnitude, const TriangleNetObject &vortonPrototype);
	void initializeVortons(const VortonSim &original);
	
	void initializeTracers(const VorticityDistribution &initialVorticity);
	void initializeTracers(const VortonSim &original);
	void setupTracerRenderProgram();

	void createOctHeap();

	void calculateVelocityGrid();
	glm::vec3 calculateVelocity(const glm::vec3 &position);

	void stretchAndTiltVortons(float seconds);

	void diffuseVorticityPSE(float seconds);
	void diffuseVorticityInside(float seconds, std::vector<Vorton*> &vortonPtrs);
	void diffuseVorticityBetween(float seconds, std::vector<Vorton*> &firstVortonPtrs, std::vector<Vorton*> &secondVortonPtrs);
	void diffuseVorticityBetween(float seconds, Vorton &first, Vorton &second);

	void advectVortons(float secondsPassed);
	void advectTracers(float secondsPassed);

	void renderTracers(const glm::mat4x4 &viewProjectTransform);

	std::pair<glm::vec3, glm::vec3> velocityGridDimensions();

	float m_Viscosity;
	float m_Density;
	std::vector<Vorton> m_Vortons;
	std::vector<Object> m_Tracers;
	std::vector<GLfloat> m_TracerVerticesRAM;
	VertexBufO m_TracerVerticesBuf;
	VertexArrO m_TracerVao;

	VortonOctHeap *m_VortonHeapPtr;
	UniformGrid<glm::vec3> *m_VelocityGridPtr;

	static const size_t m_VORTONS_PER_DIMENSION;
	static const size_t m_TRACERS_PER_DIMENSION;

	bool m_VortonsRendered;
	bool m_TracersRendered;
	bool m_Simulating;

	Program m_TracerRenderProg;
};

