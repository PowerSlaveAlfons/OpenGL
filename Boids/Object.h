#pragma once
#include "Model.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ControlService.h"
#include <glm/gtx/rotate_vector.hpp>

class Object
{
public:
	Model model;
	glm::vec3 position;
	GLuint textureId;
	GLuint texture;

	Object(Model modelNew, glm::vec3 positionNew, GLuint textureIdNew, GLuint textureNew)
	{
		model = modelNew;
		position = positionNew;
		textureId = textureIdNew;
		texture = textureNew;

		if (model.id == "Hammer.obj")
		{
			movementDirection = glm::vec3(0, 1, 0);
		}
	}

	void handlePhysics(ControlService* cService)
	{
		if (model.id == "Hammer.obj")
		{
			//movementDirection = glm::normalize(glm::vec3(cService->position - glm::vec3(position)));

			//movementDirection = movementDirection * speed;
			if (cService->getKey(GLFW_KEY_X))
				setMovement(glm::rotateZ(movementDirection, 0.001f), 0.2f);
			position = position + (movementDirection / 500.0f) * speed;

			speed = speed * drag;
			if (speed < 0)
				speed = 0;
		}

	}

	void setMovement(glm::vec3 direction, float speedNew)
	{
		movementDirection = direction;
		speed = speedNew;
	}

private:
	glm::vec3 movementDirection;
	float speed = 1.0f;
	const float drag = 0.9999f;
	bool init = false;
};