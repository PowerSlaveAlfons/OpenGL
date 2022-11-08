#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ControlService.h"
#include "ObjectLoader.h"


class Renderer {

public:
	static Renderer& getInstance() {
		static Renderer instance;
		return instance;
	}
	static GLuint loadBMP_custom(const char* imagepath);
	static GLuint loadDDS(const char* imagepath);

	static void setControlService(ControlService& service)
	{
		cService = &service;
	}

	void initialize();
	void run();

	void rotateMesh();

	typedef struct model {
		std::vector< glm::vec3 > vertices;
		std::vector< glm::vec2 > uvs;
		std::vector< glm::vec3 > normals;
	} model;

private:
	Renderer() = default;
	~Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	//GLuint vertexBuffer[3]{}, uvbuffer[3]{}, normalBuffer[3]{};

	std::vector<GLuint> vertexBuffer, uvBuffer, normalBuffer;

	//GLuint programID{};
	//GLuint matrixID{};

	GLFWwindow* window = nullptr;
	static ControlService* cService;
	const int WINDOW_WIDTH = 1920;
	const int WINDOW_HEIGHT = 1080;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void error_callback(int error, const char* description)
	{
		std::cerr << "Error: " << error << " " << description << std::endl;
	}
	GLuint loadShaders(const char* vertex_file_path, const char* fragment_file_path);
	bool loadModel(std::string FileName, std::vector<Renderer::model>& models);
};

