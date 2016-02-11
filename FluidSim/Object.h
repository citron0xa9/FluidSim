#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Scene;

class Object
{
public:
	Object(Scene &scene);
	virtual ~Object();

	virtual Object* copy() const = 0;

	virtual void render(const glm::mat4x4 &viewProjectTransform) const = 0;

	virtual void translate(const glm::vec3 &delta);
	void rotateLocalX(float degrees);
	void rotateLocalY(float degrees);

	void addForwardVelocity(float velocity);
	void addBackwardVelocity(float velocity);
	void addLeftVelocity(float velocity);
	void addRightVelocity(float velocity);

	void step(float secondsPassed);

protected:
	static glm::vec3 xAxis();
	static glm::vec3 yAxis();
	static glm::vec3 zAxis();

	static glm::vec3 forwardVector();
	static glm::vec3 leftVector();
	static glm::vec3 backwardVector();
	static glm::vec3 rightVector();

	static glm::vec3 origin();

	virtual void rotate(float degrees, const glm::vec3 &axis);

	glm::mat4x4 m_RotationTransform;
	glm::mat4x4 m_TranslationTransform;

private:

	void addLocalVelocity(const glm::vec3 &velocity);
	void updatePosition(float secondsPassed);

	Scene &m_Scene;
	glm::vec3 m_LocalVelocity;
};

