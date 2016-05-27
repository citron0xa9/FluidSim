#include "TriangleNetObject.h"

TriangleNetObject::TriangleNetObject(Material *materialPtr, Geometry *geometryPtr, Program *renderProgPtr)
	: m_MaterialPtr{ materialPtr }, m_GeometryPtr{ geometryPtr }, m_ProgramPtr{renderProgPtr}
{
}

TriangleNetObject::~TriangleNetObject()
{
}

Object * TriangleNetObject::copy() const
{
	return new TriangleNetObject{ *this };
}

const Program& TriangleNetObject::program() const
{
	return *m_ProgramPtr;
}

void TriangleNetObject::program(Program * programPtr)
{
	m_ProgramPtr = programPtr;
}

const Material & TriangleNetObject::material() const
{
	return *m_MaterialPtr;
}

void TriangleNetObject::material(Material * materialPtr)
{
	m_MaterialPtr = materialPtr;
}

const Geometry & TriangleNetObject::geometry() const
{
	return *m_GeometryPtr;
}

void TriangleNetObject::geometry(Geometry * geometryPtr)
{
	m_GeometryPtr = geometryPtr;
}

void TriangleNetObject::render(const glm::mat4x4 &viewProjectTransform)
{
	if (m_ProgramPtr == nullptr || m_GeometryPtr == nullptr || m_MaterialPtr == nullptr) {
		throw std::runtime_error("TriangleNetObject: Can't render because needed Pointer is missing");
	}
	if (m_ProgramPtr->loadedMaterialId() != m_MaterialPtr->id()) {
		m_ProgramPtr->loadMaterial(*m_MaterialPtr);
	}
	glm::mat4x4 modelTransform = m_TranslationTransform * m_ScaleTransform * m_RotationTransform;
	m_ProgramPtr->loadModelViewProjectTransform(viewProjectTransform*modelTransform);
	m_ProgramPtr->loadModelTransform(modelTransform);
	m_ProgramPtr->use();

	m_GeometryPtr->render();
}
