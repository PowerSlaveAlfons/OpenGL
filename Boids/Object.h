#pragma once
#include "Model.h"
#include <glm/glm.hpp>
#include <GL/glew.h>

class Object
{
public:
	Model model;
	glm::mat4 position;
	GLuint textureId;
	GLuint texture;

	Object(Model modelNew, glm::mat4 positionNew, GLuint textureIdNew, GLuint textureNew)
	{
		model = modelNew;
		position = positionNew;
		textureId = textureIdNew;
		texture = textureNew;
	}
};