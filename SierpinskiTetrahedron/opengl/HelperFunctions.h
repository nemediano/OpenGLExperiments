#ifndef HELPER_FUNCTIONS_H_
#define HELPER_FUNCTIONS_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <string>

using namespace std;

namespace opengl {
	void printShaderInfoLog(GLuint object);
	void printProgramInfoLog(GLuint object);
	void get_error_log();
	void gl_error(string text);
	void gl_error();
	string get_OpenGL_info();
	//Debug functions
	void print_matrix_debug(glm::mat4 A);
	void print_matrix_debug(glm::mat3 A);
	void print_vector_debug(glm::vec4 u);
	void print_vector_debug(glm::vec3 u);
}

#endif