#include "Renderer.h"
#include <filesystem>
#include <numeric>
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS



ControlService* Renderer::cService = nullptr;

GLuint Renderer::loadBMP_custom(const char* imagepath)
{
	uint8_t header[54]; // 54 Byte Header each BMP file has
	// uint32_t data_pos; // Pointer to the start of data
	uint32_t width, height;
	uint32_t image_size; // width*height*3
	uint8_t* data; // the actual image data

	FILE* file = fopen(imagepath, "rb");
	if (!file)
	{
		std::cout << "Failed to load bmp, cancelling." << std::endl;
		return false;
	}

	if (fread(header, 1, 54, file) != 54)
	{
		std::cout << "Not an actual .bmp file, cancelling." << std::endl;
		fclose(file);
		return false;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		std::cout << "Not an actual .bmp file, cancelling." << std::endl;
		fclose(file);
		return false;
	}


	// gets relevant information from the bmp file
	//data_pos = *(int*) & (header[0x0A]); // not quite sure why we need this.
	image_size = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);


	std::cout << "Read bmp file " << imagepath << std::endl;
	std::cout << width << " wide and " << height << " high" << std::endl;

	if (!image_size)
		image_size = width * height * 3;
	/*if (!data_pos)
		data_pos = 54; // Defined in the standard
*/

	data = new uint8_t[image_size];
	fread(data, 1, image_size, file); //read all the data
	fclose(file);

	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);


	return textureID;
}

GLuint Renderer::loadDDS(const char* imagepath)
{
	uint8_t header[124]; // defined by the standard

	FILE* fp;

	fp = fopen(imagepath, "rb");
	if (fp == NULL)
	{
		std::cout << "DDS file read error" << std::endl;
		return 0;
	}

	char filecode[4]{ 0 };
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS", 3) != 0) // C-Style because the tutorial ... or I don't even know
	{
		fclose(fp);
		std::cout << "DDS file read error" << std::endl;
		printf("DDS file read error");
		return 0;
	}

	fread(&header, 124, 1, fp);

	uint32_t height = *(uint32_t*)&(header[8]);
	uint32_t width = *(uint32_t*)&(header[12]);
	uint32_t linear_size = *(uint32_t*)&(header[16]);
	uint32_t mip_map_count = *(uint32_t*)&(header[24]);
	uint32_t four_cc = *(uint32_t*)&(header[80]);

	uint8_t* buffer;
	uint32_t buffer_size;

	buffer_size = mip_map_count > 1 ? linear_size * 2 : linear_size;
	buffer = (uint8_t*)malloc(buffer_size * sizeof(uint8_t));

	if (!buffer)
		return 12; // TODO: Throw error message that will probably not happen, ever
	fread(buffer, 1, buffer_size, fp);
	fclose(fp);

	//uint32_t components = (four_cc == 0x31545844) ? 3 : 4; // 0x31545844 = DXT1 in ascii
	uint32_t format;

	switch (four_cc)
	{
		case 0x31545844: // DXT1
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case 0x33545844: // DXT3
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case 0x35545844: // DXT5
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			free(buffer);
			return 0;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	uint32_t block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	uint32_t offset = 0;

	// loading mipmaps

	for (uint32_t level = 0; level < mip_map_count && (width || height); ++level)
	{
		uint32_t size = ((width + 3) / 4) * ((height + 3) / 4) * block_size;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// if texture isn't power of 2:
		if (width < 1) width = 1;
		if (height < 1) height = 1;
	}
	free(buffer);
	std::cout << "Loading Texture" << imagepath << std::endl;
	return textureID;
}

// TODO Refactor this and determine the correct order of function calls
void Renderer::initialize()
{
	std::cout << "Initializing GLFW..." << std::endl;


	Renderer::setControlService(ControlService::getInstance());
	Renderer::cService->initialize();

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) // GLFW did not successfully initialize
	{
		exit(EXIT_FAILURE);
	}

	// GLFW successfully initialized

	glfwWindowHint(GLFW_SAMPLES, 4); //4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Game", NULL, NULL);

	if (!window) // Window creation failed
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	ControlService::setWindow(window);
	glfwSetWindowPos(window, 1000, 300);

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);

	// Should we use a loader library, this is where to initialize it
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2); // TODO: Fix this meme
	glfwSwapInterval(0);

	std::cout << "Successfully initialized GLFW" << std::endl;
}


glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

void Renderer::run()
{

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f); // Set Background color

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Tells the gpu to not draw triangles behind the camera
	glEnable(GL_CULL_FACE);

	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = this->loadShaders("SimpleVertexShader.glsl", "SimpleFragmentShader.glsl");

	// Get a handle for our "MVP" uniform
	matrixID = glGetUniformLocation(programID, "MVP");
	viewMatrixID = glGetUniformLocation(programID, "V");
	modelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	GLuint texture = loadDDS("BallTexture.DDS");

	// Get a handle for our "Texture" uniform
	GLuint textureId = glGetUniformLocation(programID, "myTextureSampler");

	// Enables Alpha Blending
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	LightID = glGetUniformLocation(programID, "LightPostion_worldspace");

	double lastTime = glfwGetTime();
	double startTimeFrames = glfwGetTime();
	double startTimePhysics = glfwGetTime();
	int nbFrames = 0;
	const double LogInterval = 2.5;

	unsigned long counter = 0;
	float iterate = 0.0f;
	int modelToShow = 1;
	float distance = 0.0f;
	const double PhysicsStep = 0.1;

	Model ModelAux;

	std::vector<double> FrameTimes;

	Renderer::loadModel("Ball.obj", ModelAux);
	Renderer::AddObject(Object(ModelAux, glm::vec3(0,-2,-15), textureId, texture, glm::vec3(0,0,0), true));

	Renderer::loadModel("Corner.obj", ModelAux);
	Renderer::AddObject(Object(ModelAux, glm::vec3(-10, -5, -15), textureId, texture, glm::vec3(0, 0, 0), false));
	Renderer::loadModel("Corner.obj", ModelAux);
	Renderer::AddObject(Object(ModelAux, glm::vec3(-10, 5, -15), textureId, texture, glm::vec3(0, 0, 0), false));
	Renderer::loadModel("Corner.obj", ModelAux);
	Renderer::AddObject(Object(ModelAux, glm::vec3(10, -5, -15), textureId, texture, glm::vec3(0, 0, 0), false));
	Renderer::loadModel("Corner.obj", ModelAux);
	Renderer::AddObject(Object(ModelAux, glm::vec3(10, 5, -15), textureId, texture, glm::vec3(0, 0, 0), false));


	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		nbFrames++;
		double deltaTimeFrames = currentTime - startTimeFrames;
		double deltaTimePhysics = currentTime - startTimePhysics;

		
		if (currentTime - lastTime >= 0.5)
		{
			FrameTimes.push_back(double(nbFrames));
			nbFrames = 0;
			lastTime = currentTime;
		}

		if (deltaTimeFrames >= LogInterval)
		{
			printf("%d\n", (int)loadedModels.size());
			printf("%.2f,", 2.0 * std::accumulate(FrameTimes.begin(), FrameTimes.end(), (double)0LL) / FrameTimes.size());
			//printf("FrameTimes in Vector: %d\n", (int)FrameTimes.size());
			FrameTimes.clear();
			startTimeFrames = currentTime;
			for (Object& o : GameObjects)
				o.isPlayer = false;
			for (int i = 0; i < 5; i++)
			{
				Renderer::loadModel("Ball.obj", ModelAux);
				Renderer::AddObject(Object(ModelAux, glm::vec3(0, -2, -15), textureId, texture, glm::vec3(0, 0, 0), true));
			}
		}
		counter++;


		// Clearing the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// User shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		cService->ComputeMatricesFromInput();

		
		distance = cService->getDistanceFromOrigin();

		if (cService->getKey(GLFW_KEY_F))
		{
			bool spawnAble = true;
			/*
			for (Object& o : GameObjects)
			{
				if (o.CheckCollission(glm::vec3(0,-2,-15)))
				{
					spawnAble = false;
					break;
				}
			} */
			if (spawnAble)
			{
				for (Object& o : GameObjects)
					o.isPlayer = false;
				Renderer::loadModel("Ball.obj", ModelAux);
				Renderer::AddObject(Object(ModelAux, glm::vec3(0,-2,-15), textureId, texture, glm::vec3(0, 0, 0), true));
			}
		}


		for (Object& o : GameObjects)
		{
			if (deltaTimePhysics > PhysicsStep)
			{
				o.handlePhysics(cService);
				for (Object& other : GameObjects)
				{
					if (o.CheckCollission(other))
						o.PerformCollission(other);

				}
				deltaTimePhysics = currentTime;
			}
			draw(o);
		}

		//draw(models[modelToShow], texture, textureId);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		/* Swap front and back buffers */
		glfwSwapBuffers(window); // this only happens once per screen frame
		//glFinish();
		glfwSetCursorPos(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		/* Poll for and process events */
		glfwPollEvents();
	}
	// TODO: FIX CLEANUP
	/*
	for (int i = 0; i < models.size(); i++)

	{
		glDeleteBuffers(1, &vertexBuffer[i]);
		glDeleteBuffers(1, &uvBuffer[i]);
		glDeleteBuffers(1, &normalBuffer[i]);
	} */
	glDeleteProgram(programID);
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &vertexArrayID);

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Renderer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

}

GLuint Renderer::loadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else
	{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		if (getchar())
			return 0;
		else return 1;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void Renderer::loadModel(std::string FileName, Model& ModelAux)
{
	ModelAux.vertices.clear();
	ModelAux.uvs.clear();
	ModelAux.normals.clear();

	
	bool res = ObjectLoader::loadOBJ(FileName.c_str(), ModelAux);

	/*
	vertexBuffer.insert(std::pair<std::string, GLuint>(FileName, 0));
	uvBuffer.insert(std::pair<std::string, GLuint>(FileName, 0));
	normalBuffer.insert(std::pair<std::string, GLuint>(FileName, 0)); */


	// Loading Vertices
	glGenBuffers(1, &ModelAux.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ModelAux.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, ModelAux.vertices.size() * sizeof(glm::vec3), &ModelAux.vertices[0], GL_STATIC_DRAW);

	// Loading UVs
	glGenBuffers(1, &ModelAux.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ModelAux.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, ModelAux.uvs.size() * sizeof(glm::vec2), &ModelAux.uvs[0], GL_STATIC_DRAW);

	// Loading Normals
	glGenBuffers(1, &ModelAux.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ModelAux.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, ModelAux.normals.size() * sizeof(glm::vec3), &ModelAux.normals[0], GL_STATIC_DRAW);

	loadedModels.push_back(ModelAux);



}

bool Renderer::draw(Object& Object)
{
	//glm::mat4 TemplateMatrix = glm::mat4(1.0);
	// Alpha
	GLuint alpha = ControlService::getAlpha() ? 1.0f : 0.3f;

	ProjectionMatrix = ControlService::getProjectionMatrix();
    ViewMatrix = ControlService::getViewMatrix();
	RotationMatrix = glm::toMat4(glm::normalize(Object.orientation));
	ModelMatrix = glm::translate(glm::mat4(1.0), Object.position) * RotationMatrix;

	glm::mat4 mvp = ProjectionMatrix * ViewMatrix * ModelMatrix;
	// Send our transformation to the currently bound shader in the "MVP" uniform
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

	// Lighting

	glm::vec3 lightPos = glm::vec3(0, 0, 20);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
	//glUniform1f(glGetUniformLocation(programID, "alpha"), alpha);
	
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Object.texture);

	// Set our texture sampler to use Texture Unit 0
	glUniform1i(Object.textureId, 0);


	// Setup Vertex Buffer
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Object.model.vertexBuffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	// Setup UV Buffer
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Object.model.uvBuffer);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	// Normal Buffer
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, Object.model.normalBuffer);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);



	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)Object.model.vertices.size());
	return true;
}

void Renderer::AddObject(Object ObjectToAdd)
{
	GameObjects.push_back(ObjectToAdd);
}