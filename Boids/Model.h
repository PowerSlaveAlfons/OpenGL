#pragma once
#include "GL/glew.h"

typedef struct Model
{
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals;
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalBuffer;

	std::string id;
} model;