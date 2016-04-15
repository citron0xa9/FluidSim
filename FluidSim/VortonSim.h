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
	~VortonSim();

	virtual void step(float secondsPassed) override;
	virtual void render(const glm::mat4x4 &viewProjectTransform) override;
	virtual void registerContainerObjectHooks() override;

	void Update(float seconds);

	void setVortonsRendered(bool areRendered);
	void setTracersRendered(bool areRendered);

	virtual Object* copy() const override;

private:
	VortonSim(const VortonSim &sim) = delete;

	void initializeVortons(const VorticityDistribution &initialVorticity, float vorticityMagnitude, const TriangleNetObject &vortonPrototype);
	void initializeTracers(const VorticityDistribution &initialVorticity);
	void setupTracerRenderProgram();

	void CreateOctHeap();

	void CalculateVelocityGrid();
	glm::vec3 CalculateVelocity(const glm::vec3 &position);

	void StretchAndTiltVortons(float seconds);

	void DiffuseVorticityPSE(float seconds);
	void DiffuseVorticityInside(float seconds, std::vector<Vorton*> &vortonPtrs);
	void DiffuseVorticityBetween(float seconds, std::vector<Vorton*> &firstVortonPtrs, std::vector<Vorton*> &secondVortonPtrs);
	void DiffuseVorticityBetween(float seconds, Vorton &first, Vorton &second);

	void advectVortons(float secondsPassed);
	void advectTracers(float secondsPassed);

	void renderTracers(const glm::mat4x4 &viewProjectTransform);

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

	Program m_TracerRenderProg;
};

