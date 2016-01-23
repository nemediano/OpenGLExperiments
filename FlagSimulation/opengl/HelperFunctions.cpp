#include "HelperFunctions.h"
#include <iostream>
using namespace std;

namespace opengl {
	void printShaderInfoLog(GLuint object) {
		int infologLength = 0;
		int charsWritten = 0;
		char *infoLog;

		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &infologLength);

		if (infologLength > 0) {
			infoLog = new char[infologLength];
			glGetShaderInfoLog(object, infologLength, &charsWritten, infoLog);
			cout << infoLog;
			delete[] infoLog;
		}
	}

	void printProgramInfoLog(GLuint object) {
		int infologLength = 0;
		int charsWritten = 0;
		char *infoLog;

		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &infologLength);

		if (infologLength > 0) {
			infoLog = new char[infologLength];
			glGetProgramInfoLog(object, infologLength, &charsWritten, &infoLog[0]);
			cout << infoLog;
			delete[] infoLog;
		}
	}

	void gl_error(string text) {
		GLenum errCode;
		const GLubyte *errString;
		if ((errCode = glGetError()) != GL_NO_ERROR) {
			cout << text << endl;
			errString = gluErrorString(errCode);
			cout << "OpenGL Error: " << errString << endl;
		}
	}

	void gl_error() {
		GLenum errCode;
		const GLubyte *errString;
		if ((errCode = glGetError()) != GL_NO_ERROR) {
			errString = gluErrorString(errCode);
			cout << "OpenGL Error: " << errString << endl;
		}
	}

	string get_OpenGL_info() {
		string log;
		cout << "Hardware specification: " << endl;
		cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
		cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
		cout << "Software specification: " << endl;
		cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
		cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
		cout << "Using GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
		int ver = glutGet(GLUT_VERSION);
		cout << "Using freeglut version: " << ver / 10000 << "." << (ver / 100) % 100 << "." << ver % 100 << endl;
		return log;
	}

	void print_matrix_debug(glm::mat4 A) {
		cout << A[0][0] << "\t" << A[0][1] << "\t" << A[0][2] << "\t" << A[0][3] << endl;
		cout << A[1][0] << "\t" << A[1][1] << "\t" << A[1][2] << "\t" << A[1][3] << endl;
		cout << A[2][0] << "\t" << A[2][1] << "\t" << A[2][2] << "\t" << A[2][3] << endl;
		cout << A[3][0] << "\t" << A[3][1] << "\t" << A[3][2] << "\t" << A[3][3] << endl;
	}

	void print_matrix_debug(glm::mat3 A) {
		cout << A[0][0] << "\t" << A[0][1] << "\t" << A[0][2] << endl;
		cout << A[1][0] << "\t" << A[1][1] << "\t" << A[1][2] << endl;
		cout << A[2][0] << "\t" << A[2][1] << "\t" << A[2][2] << endl;
	}

	void print_vector_debug(glm::vec3 u) {
		cout << "(" << u.x << ", " << u.y << ", " << u.z << ")" << endl;
	}

	void print_vector_debug(glm::vec4 u) {
		cout << "(" << u.x << ", " << u.y << ", " << u.z << ", " << u.w << ")" << endl;
	}

	void APIENTRY openglCallbackFunction(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam){

		cout << "---------------------opengl-callback-start------------" << endl;
		cout << "message: " << message << endl;
		cout << "type: ";
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			cout << "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			cout << "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			cout << "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			cout << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			cout << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			cout << "OTHER";
			break;
		}
		cout << endl;

		cout << "id: " << id << endl;
		cout << "severity: ";
		switch (severity){
		case GL_DEBUG_SEVERITY_LOW:
			cout << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			cout << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			cout << "HIGH";
			break;
		}
		cout << endl;
		cout << "---------------------opengl-callback-end--------------" << endl;
	}

	void get_error_log() {
		glutInitContextFlags(GLUT_FORWARD_COMPATIBLE
#if _DEBUG
			| GLUT_DEBUG
#endif
			);

#if _DEBUG
		if (glDebugMessageCallback){
			cout << "Register OpenGL debug callback " << endl;
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(openglCallbackFunction, nullptr);
			GLuint unusedIds = 0;
			glDebugMessageControl(GL_DONT_CARE,
				GL_DONT_CARE,
				GL_DONT_CARE,
				0,
				&unusedIds,
				true);
		}
		else
			cout << "glDebugMessageCallback not available" << endl;
#endif
	}
}
