/*
	Various utility types and functions.
*/

#pragma once

#define GLEW_STATIC 1
#include <GL/glew.h> 

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Transformations {
	glm::mat4 translation;
	glm::mat4 rotation;
	glm::mat4 scaling;
};

struct Rotation {
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
};

struct Colors {
	glm::vec3 front;
	glm::vec3 left;
	glm::vec3 right;
	glm::vec3 back;
	glm::vec3 top;
	glm::vec3 bottom;
};

// Shader variable setters
inline void SetUniformMat4(GLuint shader_id, const char* uniform_name, glm::mat4 uniform_value)
{
	glUseProgram(shader_id);
	glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

inline void SetUniformVec3(GLuint shader_id, const char* uniform_name, glm::vec3 uniform_value)
{
	glUseProgram(shader_id);
	glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, glm::value_ptr(uniform_value));
}

template <class T>
inline void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value)
{
	glUseProgram(shader_id);
	glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
}