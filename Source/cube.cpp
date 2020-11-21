#include "cube.h"
#include "utils.h"

#define GLEW_STATIC 1
#include <GL/glew.h> 

#include <GLFW/glfw3.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float Cube::CUBE_WIDTH = 1.0f;
const float Cube::CUBE_HEIGHT = 1.0f;
const float Cube::CUBE_DEPTH = 1.0f;

Cube::Cube(glm::vec3 position)
{
	glm::vec3 yellow = glm::vec3(0.988f, 0.91f, 0.31f);
	glm::vec3 red = glm::vec3(0.988f, 0.22f, 0.22f);
	glm::vec3 blue = glm::vec3(0.141f, 0.357f, 1.0f);
	glm::vec3 orange = glm::vec3(1.0f, 0.58f, 0.212f);
	glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 green = glm::vec3(0.694f, 0.949f, 0.38f);
	colors.front = red;
	colors.left = green;
	colors.right = blue;
	colors.back = orange;
	colors.top = yellow;
	colors.bottom = white;

	this->cubeTransformations.translation = glm::mat4(1.0f);
	this->cubeTransformations.scaling = glm::mat4(1.0f);
	this->cubeTransformations.rotation = glm::mat4(1.0f);
	this->CreateVertexArrayObject();
	this->SetPosition(position);
	pivot = glm::vec3(0.0f, 0.0f, 0.0f);
	isSelected = false;
}

Cube::~Cube()
{
	glDeleteVertexArrays(1, &vao);
}

void Cube::CreateVertexArrayObject()
{
	glm::vec3 cubeVertices[] = {
		glm::vec3(CUBE_WIDTH,  CUBE_HEIGHT, 0.0f), // 0. front - top right
		glm::vec3(0.0f,  CUBE_HEIGHT, 0.0f), // 1. front - top left
		glm::vec3(CUBE_WIDTH, 0.0f, 0.0f), // 2. front - bottom right
		glm::vec3(0.0f, 0.0f, 0.0f), // 3. front - bottom left

		glm::vec3(CUBE_WIDTH,  CUBE_HEIGHT, CUBE_DEPTH), // 4. back - top right
		glm::vec3(0.0f,  CUBE_HEIGHT, CUBE_DEPTH), // 5. back - top left
		glm::vec3(CUBE_WIDTH, 0.0f, CUBE_DEPTH), // 6. back - bottom right
		glm::vec3(0.0f, 0.0f, CUBE_DEPTH), // 7. back - bottom left
	};

	float vertexArray[] = {
		// FRONT
		cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z, colors.front.x, colors.front.y, colors.front.z,
		cubeVertices[3].x, cubeVertices[3].y, cubeVertices[3].z, colors.front.x, colors.front.y, colors.front.z,
		cubeVertices[2].x, cubeVertices[2].y, cubeVertices[2].z, colors.front.x, colors.front.y, colors.front.z,

		cubeVertices[3].x, cubeVertices[3].y, cubeVertices[3].z, colors.front.x, colors.front.y, colors.front.z,
		cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z, colors.front.x, colors.front.y, colors.front.z,
		cubeVertices[1].x, cubeVertices[1].y, cubeVertices[1].z, colors.front.x, colors.front.y, colors.front.z,

		// LEFT
		cubeVertices[1].x, cubeVertices[1].y, cubeVertices[1].z, colors.left.x, colors.left.y, colors.left.z,
		cubeVertices[7].x, cubeVertices[7].y, cubeVertices[7].z, colors.left.x, colors.left.y, colors.left.z,
		cubeVertices[3].x, cubeVertices[3].y, cubeVertices[3].z, colors.left.x, colors.left.y, colors.left.z,

		cubeVertices[7].x, cubeVertices[7].y, cubeVertices[7].z, colors.left.x, colors.left.y, colors.left.z,
		cubeVertices[1].x, cubeVertices[1].y, cubeVertices[1].z, colors.left.x, colors.left.y, colors.left.z,
		cubeVertices[5].x, cubeVertices[5].y, cubeVertices[5].z, colors.left.x, colors.left.y, colors.left.z,

		// RIGHT
		cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z, colors.right.x, colors.right.y, colors.right.z,
		cubeVertices[2].x, cubeVertices[2].y, cubeVertices[2].z, colors.right.x, colors.right.y, colors.right.z,
		cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z, colors.right.x, colors.right.y, colors.right.z,

		cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z, colors.right.x, colors.right.y, colors.right.z,
		cubeVertices[4].x, cubeVertices[4].y, cubeVertices[4].z, colors.right.x, colors.right.y, colors.right.z,
		cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z, colors.right.x, colors.right.y, colors.right.z,

		// BACK
		cubeVertices[4].x, cubeVertices[4].y, cubeVertices[4].z, colors.back.x, colors.back.y, colors.back.z,
		cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z, colors.back.x, colors.back.y, colors.back.z,
		cubeVertices[5].x, cubeVertices[5].y, cubeVertices[5].z, colors.back.x, colors.back.y, colors.back.z,

		cubeVertices[7].x, cubeVertices[7].y, cubeVertices[7].z, colors.back.x, colors.back.y, colors.back.z,
		cubeVertices[5].x, cubeVertices[5].y, cubeVertices[5].z, colors.back.x, colors.back.y, colors.back.z,
		cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z, colors.back.x, colors.back.y, colors.back.z,
		
		// TOP
		cubeVertices[1].x, cubeVertices[1].y, cubeVertices[1].z, colors.top.x, colors.top.y, colors.top.z,
		cubeVertices[0].x, cubeVertices[0].y, cubeVertices[0].z, colors.top.x, colors.top.y, colors.top.z,
		cubeVertices[4].x, cubeVertices[4].y, cubeVertices[4].z, colors.top.x, colors.top.y, colors.top.z,

		cubeVertices[4].x, cubeVertices[4].y, cubeVertices[4].z, colors.top.x, colors.top.y, colors.top.z,
		cubeVertices[5].x, cubeVertices[5].y, cubeVertices[5].z, colors.top.x, colors.top.y, colors.top.z,
		cubeVertices[1].x, cubeVertices[1].y, cubeVertices[1].z, colors.top.x, colors.top.y, colors.top.z,

		// BOTTOM
		cubeVertices[3].x, cubeVertices[3].y, cubeVertices[3].z, colors.bottom.x, colors.bottom.y, colors.bottom.z,
		cubeVertices[7].x, cubeVertices[7].y, cubeVertices[7].z, colors.bottom.x, colors.bottom.y, colors.bottom.z,
		cubeVertices[2].x, cubeVertices[2].y, cubeVertices[2].z, colors.bottom.x, colors.bottom.y, colors.bottom.z,

		cubeVertices[7].x, cubeVertices[7].y, cubeVertices[7].z, colors.bottom.x, colors.bottom.y, colors.bottom.z,
		cubeVertices[6].x, cubeVertices[6].y, cubeVertices[6].z, colors.bottom.x, colors.bottom.y, colors.bottom.z,
		cubeVertices[2].x, cubeVertices[2].y, cubeVertices[2].z, colors.bottom.x, colors.bottom.y, colors.bottom.z,
	};

	GLuint vertexArrayObject;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	GLuint vertexBufferObject;
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		6*sizeof(float),
		(void*)0
	);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),
		(void*)(3 * sizeof(float))
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	this->vao = vertexArrayObject;
}

void Cube::Draw(Transformations parentTransformations)
{
	glBindVertexArray(vao);

	glm::mat4 cubeTranslationMatrix = cubeTransformations.translation * glm::translate(glm::mat4(1.0f), position);

	glm::vec3 distanceWithGlobalPivot = glm::vec3(1.5f, 1.5f, 1.5f);
	glm::mat4 parentRotationMatrix = glm::translate(glm::mat4(1.0f), distanceWithGlobalPivot) * parentTransformations.rotation * glm::translate(glm::mat4(1.0f), -distanceWithGlobalPivot);

	glm::mat4 cubeTransformationsMatrix = cubeTranslationMatrix * cubeTransformations.rotation * cubeTransformations.scaling;
	glm::mat4 parentTransformationsMatrix = parentTransformations.translation * parentRotationMatrix * parentTransformations.scaling;

	glm::mat4 cubeWorldMatrix = parentTransformationsMatrix * cubeTransformationsMatrix;

	SetUniformMat4(shader, "worldMatrix", cubeWorldMatrix);

	if (this->isSelected) {
		SetUniform1Value(shader, "useHighlightColor", true);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		SetUniform1Value(shader, "useHighlightColor", false);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glBindVertexArray(0);
}

void Cube::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void Cube::SetColor(Colors colors)
{
	this->colors.front = colors.front;
	this->colors.left = colors.left;
	this->colors.right = colors.right;
	this->colors.back = colors.back;
	this->colors.top = colors.top;
	this->colors.bottom = colors.bottom;
}

void Cube::SetPivot(glm::vec3 pivot)
{
	this->pivot = pivot;
}

void Cube::SetIsSelected(bool isSelected)
{
	this->isSelected = isSelected;
}

void Cube::SetTranslation(glm::mat4 translation)
{
	this->cubeTransformations.translation = translation;
}

void Cube::SetScaling(glm::mat4 scaling)
{
	this->cubeTransformations.scaling = scaling;
}

void Cube::SetRotation(glm::mat4 rotation)
{
	this->cubeTransformations.rotation = rotation;
}

void Cube::SetShader(GLuint shader)
{
	this->shader = shader;
}
