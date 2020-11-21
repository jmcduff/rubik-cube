#include "utils.h"

#define GLEW_STATIC 1
#include <GL/glew.h> 

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// shader variable setters
void SetUniformMat4(GLuint shader_id, const char* uniform_name, glm::mat4 uniform_value)
{
	glUseProgram(shader_id);
	glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

void SetUniformVec3(GLuint shader_id, const char* uniform_name, glm::vec3 uniform_value)
{
	glUseProgram(shader_id);
	glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, glm::value_ptr(uniform_value));
}

template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value)
{
	glUseProgram(shader_id);
	glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
	glUseProgram(0);
}