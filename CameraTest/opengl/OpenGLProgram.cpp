#include <iostream>
#include <stdlib.h>
#include <fstream>

#include <GL/glew.h>
#include "OpenGLProgram.h"
#include "HelperFunctions.h"

namespace opengl {

	OpenGLProgram::OpenGLProgram(string vertex_shader_file_name, string fragment_shader_file_name) {
		/* Cool preprocessing */
		m_program_ok = true;
		ifstream input_vertex_shader_file(vertex_shader_file_name, ios::in | ios::binary);
		if (input_vertex_shader_file) {
			input_vertex_shader_file.seekg(0, ios::end);
			m_vertex_shader_source.resize(static_cast<unsigned int>(input_vertex_shader_file.tellg()));
			input_vertex_shader_file.seekg(0, ios::beg);
			input_vertex_shader_file.read(&m_vertex_shader_source[0], m_vertex_shader_source.size());
			input_vertex_shader_file.close();
		} else {
			cerr << "Could not open vertex shader file at: " << vertex_shader_file_name << endl;
			m_program_ok = false;
		}

		ifstream input_fragment_shader_file(fragment_shader_file_name, ios::in | ios::binary);
		if (input_fragment_shader_file) {
			input_fragment_shader_file.seekg(0, ios::end);
			m_fragment_shader_source.resize(static_cast<unsigned int>(input_fragment_shader_file.tellg()));
			input_fragment_shader_file.seekg(0, ios::beg);
			input_fragment_shader_file.read(&m_fragment_shader_source[0], m_fragment_shader_source.size());
			input_fragment_shader_file.close();
		} else {
			cerr << "Could not open fragment shader file at: " << fragment_shader_file_name << endl;
			m_program_ok = false;
		}

		/* or not */
		m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		const char* start = &m_vertex_shader_source[0];
		glShaderSource(m_vertex_shader, 1, &start, NULL);

		m_fragment_shader= glCreateShader(GL_FRAGMENT_SHADER);
		start = &m_fragment_shader_source[0];
		glShaderSource(m_fragment_shader, 1, &start, NULL);

		try {
			int status;
			glCompileShader(m_vertex_shader);
			glGetShaderiv(m_vertex_shader, GL_COMPILE_STATUS, &status); 

			if(status == GL_FALSE) {
				throw m_vertex_shader;
			}

			glCompileShader(m_fragment_shader);
			glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &status); 
	
			if(status == GL_FALSE) {
				throw m_fragment_shader;
			}
		} catch(GLuint bad_shader) {
			printShaderInfoLog(bad_shader);
			m_program_ok = false;
		}

		m_program = glCreateProgram();
		try {
			int status;
			glAttachShader(m_program, m_vertex_shader);
			glAttachShader(m_program, m_fragment_shader);
			glLinkProgram(m_program);

			glGetProgramiv(m_program, GL_LINK_STATUS, &status);

			if(status == GL_FALSE) {
				throw m_program;	
			}

		} catch(GLuint bad_program) {
			printProgramInfoLog(bad_program);
			m_program_ok = false;
		}

	}
	
	OpenGLProgram::~OpenGLProgram() {
		glDetachShader(m_program, m_vertex_shader); 
		glDetachShader(m_program, m_fragment_shader);

		glDeleteShader(m_vertex_shader); 
		glDeleteShader(m_fragment_shader); 
		glDeleteProgram(m_program); 
	}

	void OpenGLProgram::use_program() {
		glUseProgram(m_program);
	}

	GLint OpenGLProgram::get_uniform_location(string variable) {
		return glGetUniformLocation(m_program, variable.c_str());
	}

	GLint OpenGLProgram::get_attrib_location(string variable) {
		return glGetAttribLocation(m_program, variable.c_str());
	}

	bool OpenGLProgram::is_ok() {
		return m_program_ok;
	}
    	
};
