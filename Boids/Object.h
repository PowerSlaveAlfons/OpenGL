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

	Object(Model modelNew, glm::vec3 positionNew, GLuint textureIdNew, GLuint textureNew, glm::quat orientationNew)
	{
		model = modelNew;
		position = positionNew;
		textureId = textureIdNew;
		texture = textureNew;
		orientation = orientationNew;
		orientationVec = glm::vec3(0, 1, 0);

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


			angle = glm::length(position - oldPosition) * speed * 50;// sphereRadius;

			//angle += glm::radians(0.0005f);
			//orientation = glm::rotate(orientation, angle, glm::cross(orientation, glm::vec3(0,0,1))) * orientation;
			//std::cout << glm::to_string(orientation) << std::endl;
			orientation = glm::angleAxis(glm::degrees(angle), glm::cross(movementDirection, glm::vec3(0, 0, 1)));
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

	glm::quat getOrientation()
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
	float angle = glm::radians(90.0f);
};