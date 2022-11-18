#pragma once
#include "Model.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "ControlService.h"

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
	}

	void handlePhysics(ControlService* cService)
	{
		if (model.id == "Hammer.obj")
		{
			glm::vec3 towards = glm::normalize(glm::vec3(cService->position - glm::vec3(position)));
			position = position + (towards / 5000.0f);
		}

	}
};