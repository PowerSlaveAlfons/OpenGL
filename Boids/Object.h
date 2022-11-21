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
	glm::vec3 orientation;

	Object(Model modelNew, glm::vec3 positionNew, GLuint textureIdNew, GLuint textureNew, glm::vec3 orientationNew)
	{
		model = modelNew;
		position = positionNew;
		textureId = textureIdNew;
		texture = textureNew;
		orientation = orientationNew;

		if (model.id == "Ball.obj")
		{
			movementDirection = glm::vec3(0, 1, 0);
		}
	}

	void handlePhysics(ControlService* cService)
	{
		if (model.id == "Ball.obj")
		{
			//movementDirection = glm::normalize(glm::vec3(cService->position - glm::vec3(position)));

			//movementDirection = movementDirection * speed;
			if (cService->getKey(GLFW_KEY_X))
				setMovement(glm::rotateZ(movementDirection, 0.001f), 1.5f);
			glm::vec3 oldPosition = position;
			position = position + (movementDirection / 500.0f) * speed;

			speed = speed * drag;
			if (speed < 0)
				speed = 0;

			if (position.x > 10 || position.x < -10)
				movementDirection.x *= -1;
			if (position.y > 10 || position.y < -10)
				movementDirection.y *= -1;


			angle = glm::length(position - oldPosition);// sphereRadius;
			orientation = glm::rotateZ(orientation, angle);
			std::cout << angle << std::endl;

		}

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

	glm::vec3 getOrientation()
	{
		return orientation;
	}

	float getAngle()
	{
		return angle;
	}

private:
	glm::vec3 movementDirection;
	float speed = 1.0f;
	const float drag = 0.9999f;
	bool init = false;
	float angle = 0.0f;
};