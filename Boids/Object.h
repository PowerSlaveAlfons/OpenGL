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
};