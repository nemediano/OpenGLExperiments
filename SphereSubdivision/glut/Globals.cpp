#include "Globals.h"
#include <glm/gtx/norm.hpp>

namespace options {

	//Scene
	GLfloat world_radious = 4.0f;
	float delta_seconds = 0.0f;

	//Camera handling
	GLfloat field_of_view_y = PI / 3.0f;
	glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, world_radious);
	glm::vec3 camera_center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec2 camera_pan = glm::vec2(0.0f, 0.0f);
	glm::quat camera_base_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glm::quat camera_new_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	
	//For the mouse dragging 
	bool mouse_dragging = false;
	glm::vec2 mouse_start_drag = glm::vec2(0.0f, 0.0f);

	//Glut window pointer
	int window = 0;

	//Variables for GPU side
	GLint u_PVM_location = -1;
	GLint u_NormalMatrix_location = -1;
	GLint u_VM_location = -1;

	GLint a_position_loc = -1;
	GLint a_normal_loc = -1;

	//Program logic
	GLint subdiv_level = 0;

	//Manage the Vertex Buffer Object
	GLuint vbo = 0;
	GLuint indexBuffer = 0;

	//OpenGL main program
	opengl::OpenGLProgram* program_ptr = nullptr;

	//Light source
	scene::Light light;
	//Shader location for light
	GLint u_LightPosition_location = -1;
	GLint u_La_location = -1;
	GLint u_Ld_location = -1;
	GLint u_Ls_location = -1;
	//Shader location for the camera
	GLint u_view = -1;

	//Material
	scene::Material material;
	GLint u_Ka_location = -1;
	GLint u_Kd_location = -1;
	GLint u_Ks_location = -1;
	GLint u_shininess_location = -1;

}