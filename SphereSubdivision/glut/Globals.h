#ifndef GLOBALS_H
#define GLOBALS_H

#include <GL/glew.h>
#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "../opengl/OpenGLProgram.h"
#include "../scene/Light.h"
#include "../scene/Material.h"

namespace options {

	//Scene
	extern GLfloat world_radious;
	extern float delta_seconds;

	//Camera handling
	extern GLfloat field_of_view_y;
	extern glm::vec3 camera_position;
	extern glm::vec3 camera_center;
	extern glm::vec3 camera_up;
	extern glm::vec2 camera_pan;
	extern glm::quat camera_base_rotation;
	extern glm::quat camera_new_rotation;
	extern GLfloat field_of_view;

	//For the mouse dragging 
	extern bool mouse_dragging;
	extern glm::vec2 mouse_start_drag;

	//Glut window pointer
	extern int window;

	//Variables for GPU side
	extern GLint u_PVM_location;
	extern GLint u_NormalMatrix_location;
	extern GLint u_VM_location;

	extern GLint a_position_loc;
	extern GLint a_normal_loc;

	//Program logic
	extern GLint subdiv_level;
	extern bool wireframe;
	extern bool icosahedron;

	//Manage the Vertex Buffer Object
	extern GLuint vbo;
	extern GLuint indexBuffer;

	//OpenGL main program
	extern opengl::OpenGLProgram* program_ptr;

	//Light source
	extern scene::Light light;
	//Shader location for light
	extern GLint u_LightPosition_location;
	extern GLint u_La_location;
	extern GLint u_Ld_location;
	extern GLint u_Ls_location;
	//Shader location for the camera
	extern GLint u_view;

	//Material
	extern scene::Material material;
	extern GLint u_Ka_location;
	extern GLint u_Kd_location;
	extern GLint u_Ks_location;
	extern GLint u_shininess_location;
}

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
};

//Two math constants (New glm uses radians as default)
const float TAU = 6.28318f;
const float PI = 3.14159f;


#endif