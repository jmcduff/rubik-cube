/*
	The main purpose of this class is to draw an individual cube.
*/

#pragma once

#define GLEW_STATIC 1
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>

#include "utils.h"

class Cube {
public:
	Cube(glm::vec3 position);
	virtual ~Cube();

	static const float CUBE_WIDTH;
	static const float CUBE_HEIGHT;
	static const float CUBE_DEPTH;

	void CreateVertexArrayObject();
	void Draw(Transformations parentTransformations);

	// Setters
	void SetPosition(glm::vec3 position);
	void SetColor(Colors colors);
	void SetPivot(glm::vec3 pivot);

	void SetIsSelected(bool isSelected);

	void SetTranslation(glm::mat4 translation);
	void SetScaling(glm::mat4 scaling);
	void SetRotation(glm::mat4 rotation);

	void SetShader(GLuint shader);

	// Getters
	glm::vec3 GetPosition() const { return position; }
	glm::vec3 GetPivot() const { return pivot; }

	bool GetIsSelected() const { return isSelected; }

	glm::mat4 GetScaling() const { return cubeTransformations.scaling; }
	glm::mat4 GetTranslation() const { return cubeTransformations.translation; }
	glm::mat4 GetRotation() const { return cubeTransformations.rotation; }

	GLuint GetShader() const { return shader; }

protected:
	bool isSelected;

	glm::vec3 position;
	glm::vec3 pivot;

	Colors colors;

	Transformations cubeTransformations; // the child transformations to apply on the cube

	GLuint shader;

private:
	GLuint vao;
};