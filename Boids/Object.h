#pragma once
#include "Model.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ControlService.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

class Object
{
public:
	Model model;
	glm::vec3 position;
	GLuint textureId;
	GLuint texture;
	glm::quat orientation;
	glm::vec3 orientationVec;
	bool isPlayer;

	Object(Model modelNew, glm::vec3 positionNew, GLuint textureIdNew, GLuint textureNew, glm::quat orientationNew, bool isPlayerNew)
	{
		model = modelNew;
		position = positionNew;
		textureId = textureIdNew;
		texture = textureNew;
		orientation = orientationNew;
		orientationVec = glm::vec3(0, 1, 0);
		isPlayer = isPlayerNew;

		if (model.id == "Ball.obj")
		{
			movementDirection = glm::vec3(0, 1, 0);
		}
	}

	void handlePhysics(ControlService* cService)
	{
		if (model.id == "Ball.obj")
		{
			if (isPlayer && cService->getKey(GLFW_KEY_X))
				setMovement(glm::rotateZ(movementDirection, 0.001f), 1.5f);
			glm::vec3 oldPosition = position;
			position = position + (glm::normalize(movementDirection) / 500.0f) * speed;

			speed = speed * drag;
			if (speed < 0)
				speed = 0;

			if (position.x > 10 || position.x < -10)
				movementDirection.x *= -1;
			if (position.y > 5 || position.y < -5)
				movementDirection.y *= -1;


			angle = glm::length(position - oldPosition) * speed * 50;
			orientation = glm::angleAxis(glm::degrees(angle), glm::cross(movementDirection, glm::vec3(0, 0, 1)));
		}

	}

	bool CheckCollission(Object& other)
	{
		if (Object::AreEqual(*this, other))
			return false;
		return glm::length(this->position - other.position) < 1.0f;
	}

	bool CheckCollission(glm::vec3 other)
	{
		return glm::length(this->position - other) < 1.0f;
	}

	void PerformCollission(Object& other)
	{
		if (other.model.id == "Corner.obj")
			position = glm::vec3(-100, -100, -100);
		
		glm::vec3 nv1 = this->movementDirection * this->speed;
		glm::vec3 nv2 = other.movementDirection * other.speed;

		nv1 += project(other.movementDirection * other.speed, other.position - this->position);
		nv1 -= project(this->movementDirection * this->speed, this->position - other.position);
		this->speed = glm::length(nv1);
		this->movementDirection = glm::normalize(nv1);

		nv2 += project(this->movementDirection * this->speed, other.position - this->position);
		nv2 -= project(other.movementDirection * other.speed, this->position - other.position);
		other.speed = glm::length(nv2);
		other.movementDirection = glm::normalize(nv2);

	}

	glm::vec3 project(const glm::vec3& u, const glm::vec3& v)
	{
		return (glm::dot(u, v) / glm::dot(v, v)) * v;
	}
	void setMovement(glm::vec3 direction, float speedNew)
	{
		movementDirection = direction;
		speed = speedNew;
	}

	glm::vec3 getMovement()
	{
		return movementDirection;
	}

	glm::quat getOrientation()
	{
		return orientation;
	}

	float getAngle()
	{
		return angle;
	}
	
	static bool AreEqual(const Object& a, const Object& b)
	{
		return &a == &b;
	}

private:
	glm::vec3 movementDirection;
	float speed = 0.0f;
	const float drag = 0.9999f;
	bool init = false;
	float angle = glm::radians(90.0f);
};