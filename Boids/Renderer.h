#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <map>
#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

#include "ControlService.h"
#include "ObjectLoader.h"
#include "Model.h"
#include "Object.h"




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
	bool draw(Object& object);
	void loadModel(std::string FileName, Model& ModelOut);
	void AddObject(Object ObjectToAdd);


	void rotateMesh();


private:
	Renderer() = default;
	~Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	std::vector<Model> loadedModels;
	std::vector<Object> GameObjects;

	std::map<std::string, GLuint> vertexBuffer, uvBuffer, normalBuffer;
	GLuint matrixID, viewMatrixID, modelMatrixID, LightID, programID;
	glm::mat4 ProjectionMatrix, ViewMatrix, ModelMatrix, mvp;

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
};

