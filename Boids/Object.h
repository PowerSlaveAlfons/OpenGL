#pragma once
#include "Model.h"
#include <glm/glm.hpp>
#include <GL/glew.h>

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

	glm::mat4 getModelMatrix()
	{
		return glm::mat4(                 1.0f, 0.0f, 0.0f, position.x,
										  0.0f, 1.0f, 0.0f, position.y,
										  0.0f, 0.0f, 1.0f, position.z,
										  0.0f, 0.0f, 0.0f, 1.0f);
	}
};