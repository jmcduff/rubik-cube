/*
	Minimalist Rubik's Cube
	by Joëlle McDuff

	This code was written in the context of Concordia University's Computer Graphics course (COMP 371).
	
	References:
		Course material from COMP 371.
		For key input detection:
			1. https://stackoverflow.com/questions/51873906/is-there-a-way-to-process-only-one-input-event-after-a-key-is-pressed-using-glfw
			2. https://discourse.glfw.org/t/passing-parameters-to-callbacks/848
			3. https://stackoverflow.com/questions/10072004/c-typecast-cast-a-pointer-from-void-pointer-to-class-pointer
*/

#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#define GLEW_STATIC 1
#include <GL/glew.h> 

#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rubik.h"
#include "utils.h"

// Referenced from COMP 371 course material
int compileAndLinkShaders(std::string vertexFilePath, std::string fragmentFilePath)
{
	// compile and link shader program
	// return shader program id
	// ------------------------------------

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexFilePath, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragmentFilePath, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	// vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSource = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSource = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

void detectKeyUserInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
	void* pointer = glfwGetWindowUserPointer(window);
	Rubik* rubik = static_cast<Rubik *>(pointer);

	if (!rubik->GetIsAnimated()) {
		/****** CHANGE SELECTION SECTION *******/
		if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
			int selectedSection = rubik->GetSelectedRubikSection();
			selectedSection++;
			if (selectedSection >= Rubik::NUMBER_OF_LAYERS) {
				selectedSection = 0;
			}

			rubik->SetSelectedRubikSection(selectedSection);
		}
		else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
			int selectedSection = rubik->GetSelectedRubikSection();
			selectedSection--;
			if (selectedSection < 0) {
				selectedSection = Rubik::NUMBER_OF_LAYERS - 1;
			}

			rubik->SetSelectedRubikSection(selectedSection);
		}
		/****** CHANGE SELECTION MODE *******/
		else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			rubik->SwitchRubikSelectedSectionType(true);
		}
		else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			rubik->SwitchRubikSelectedSectionType(false);
		}
		/****** ROTATE SELECTED SECTION (FOWARD) *******/
		else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
			rubik->RotateRubikSelectedSection(true);
		}
		/****** ROTATE SELECTED SECTION (BACKWARD) *******/
		else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
			rubik->RotateRubikSelectedSection(false);
		}
	}
}

int main(int argc, char*argv[])
{
	// Initialize GLFW and OpenGL version
	glfwInit();

#if defined(PLATFORM_OSX)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
	// On windows, we set OpenGL version to 2.1, to support more hardware
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

	// Create Window and rendering context using GLFW, resolution is 1024x768
	const int WINDOW_WIDTH = 1024;
	const int WINDOW_HEIGHT = 768;
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rubik's Cube", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to create GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Setting background color
	glClearColor(1.0f, 1.0f, 0.709f, 0.0f);

	// Compile and link shaders
	int shaderProgram = compileAndLinkShaders("../Source/VertexShader.glsl", "../Source/FragmentShader.glsl");

	// Don't use highlight color by default
	SetUniform1Value(shaderProgram, "useHighlightColor", false);

	// Enable Backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	// Enable Depth testing
	glEnable(GL_DEPTH_TEST);

	/******  CAMERA VARIABLES  *****/
	glm::vec3 cameraPosition = glm::vec3(1.5f, 3.5f, 10.0f);
	glm::vec3 cameraCenter = glm::vec3(1.5f, 3.5f, 0.0f);

	glm::mat4 viewMatrix = glm::lookAt(cameraPosition, // eye
		cameraCenter, // center
		glm::vec3(0.0f, 1.0f, 0.0f));

	/********* SET UP SCENE OBJECTS *********/
	Rubik *rubik = new Rubik(glm::vec3(0.0f, 2.0f, 0.0f), shaderProgram);

	/*********** SET UP KEY INPUT DETECTION ************/
	glfwSetWindowUserPointer(window, rubik);
	glfwSetKeyCallback(window, detectKeyUserInput);

	/*********** SET UP FRAME TIME TRACKING ************/
	float lastFrameTime = glfwGetTime();

	// Main Loop
	while (!glfwWindowShouldClose(window))
	{
		// Frame time calculation
		float dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;

		/******** SCENE RENDERING ********/
		glUseProgram(shaderProgram);

		// Set view position on scene shader
		SetUniformVec3(shaderProgram, "view_position", cameraPosition);

		// Use proper image output size
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Bind screen as output framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		rubik->SetShader(shaderProgram);
		rubik->Draw();

		// Adjusting perspective
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.01f, 100.0f);
		SetUniformMat4(shaderProgram, "projectionMatrix", projectionMatrix);

		/*************************
		MOUSE BUTTONS USER INPUT
		*************************/
		// Calculate mouse position in X and Y
		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		/****** ROTATING THE RUBIK'S CUBE ON THE Y AXIS *******/
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			const float signX = mousePosX > (WINDOW_HEIGHT / 2) ? 1.0f : -1.0f;

			glm::mat4 newRotation = glm::rotate(glm::mat4(1.0f), glm::radians(70.0f * dt * signX), glm::vec3(0.0f, 1.0f, 0.0f));
			newRotation = newRotation * rubik->GetRotation();

			rubik->SetRotation(newRotation);
		}

		/****** ROTATING THE RUBIK'S CUBE ON THE X AXIS *******/
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			const float signY = mousePosY > (WINDOW_HEIGHT / 2) ? 1.0f : -1.0f;

			glm::mat4 newRotation = glm::rotate(glm::mat4(1.0f), glm::radians(70.0f * dt * signY), glm::vec3(1.0f, 0.0f, 0.0f));
			newRotation = newRotation * rubik->GetRotation();

			rubik->SetRotation(newRotation);
		}

		/****** ADJUSTING CAMERA POSITION *******/
		viewMatrix = glm::lookAt(cameraPosition, // eye
			cameraCenter, // center
			glm::vec3(0.0f, 1.0f, 0.0f));
		SetUniformMat4(shaderProgram, "viewMatrix", viewMatrix);

		/****** DETECT EXIT *******/
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		// End Frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Shutdown GLFW
	glfwTerminate();

	return 0;
}
