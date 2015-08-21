#include <GL/glew.h>
#include <GL/freeglut.h>

//This is a bug on GLM in 32 bits I hope they solve it soon
#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <cstdlib>
#include <cmath>
#include <iostream>

#include <IL/il.h>
#include <IL/ilu.h>

#include "OpenGLProgram.h"
#include "SceneCreation.h"
#include "TextureHandler.h"

using namespace std;
using namespace shaders;
using namespace scene;
using namespace texture;

static int win = 0;

OpenGLProgram* gl_program_ptr = nullptr;
SceneCreation* scene_creation_ptr = nullptr;
TextureHandler* texture_stone_ptr = nullptr;
TextureHandler* texture_unam_ptr = nullptr;

//Program management
void exit_glut();
void init_OpenGL();
void init_program();
void create_glut_window();
void create_glut_callbacks();
void reset_camera();

//Callback function
void display();
void reshape(int new_window_width, int new_window_height);
void mouse (int button, int state, int mouse_x, int mouse_y);
void keyboard(unsigned char key, int mouse_x, int mouse_y);
void special_keyboard(int key, int mouse_x, int mouse_y);
void mouse_active(int mouse_x, int mouse_y);
void mouse(int button, int state, int mouse_x, int mouse_y);
void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y);
void idle();

//Program logic handler
bool scene_rotation_flag;
enum BlendingMode{OFF, ALPHA_BLENDING, ADDITIVE_BLENDING};
BlendingMode display_mode;
bool texture_mapping_flag;
float scene_rotation_angle;

//Two math constants (New glm uses radians as default)
const float TAU = 6.2831853071796f;
const float PI = 3.1415926535898f;

GLint u_M_location = -1;
GLint u_V_location = -1;
GLint u_P_location = -1;
GLint u_color_location = -1;
GLint texture_option_loc = -1;

GLint a_position_loc = -1;
GLint a_normal_loc = -1;
GLint a_text_coordinates_loc = -1;

float seconds;
int window_width;
int window_height;
double aspect_ratio;
float world_radious;

//Camera control
glm::vec3 eye;
glm::vec3 at;
glm::vec3 up;
bool mouse_dragging;
glm::vec2 mouse_start_drag;
glm::vec2 base_rotation_angles;
glm::vec2 new_rotation_angles;
GLdouble scale_perspective;
GLdouble scale_ortho;
bool projection_type;



void init_OpenGL() {
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
	}
	cout << "Hardware specification: " << endl;
	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Software specification: " << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	int ver = glutGet(GLUT_VERSION);
	cout << "Using freeglut version: " <<  ver / 10000 << "." << (ver / 100) % 100 << "." << ver % 100 << endl;

	gl_program_ptr = new OpenGLProgram("proy4_vs.glsl", "proy4_fs.glsl");
	if (!gl_program_ptr->is_ok()) {
		cerr << "Error at GL program creation" << endl;
		gl_error();
		exit(EXIT_FAILURE);
	}

	texture_stone_ptr = new TextureHandler();
	texture_stone_ptr->load_texture(L"img/stone.png");
	texture_unam_ptr = new TextureHandler();
	texture_unam_ptr->load_texture(L"img/unam.png");

	u_M_location = gl_program_ptr->get_uniform_location("M");
	u_V_location = gl_program_ptr->get_uniform_location("V");
	u_P_location = gl_program_ptr->get_uniform_location("P");
	u_color_location = gl_program_ptr->get_uniform_location("Color");

	a_position_loc = gl_program_ptr->get_attrib_location("Position");
	a_normal_loc = gl_program_ptr->get_attrib_location("Normal");;
	a_text_coordinates_loc = gl_program_ptr->get_attrib_location("TextCoord");

	//set texture unit for uniform sampler variable
	int tex_loc = gl_program_ptr->get_uniform_location("texture_map");
	texture_option_loc = gl_program_ptr->get_uniform_location("texture_option");
	if(tex_loc != -1) {
		glUniform1i(tex_loc, 0); // we will bind our texture to texture unit 0
	}
	
	
	//Activate antialliasing
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	//initialize some basic rendering state
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	gl_error();

	//Create scene
	scene_creation_ptr = new SceneCreation();
	gl_error("At scene creation");
}

void display() {
	gl_program_ptr->use_program();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Init all matrices at Identity to be sure
	glm::mat4 M = glm::mat4(1.0);
	glm::mat4 V = glm::mat4(1.0);
	glm::mat4 P = glm::mat4(1.0);
	glm::mat4 I = glm::mat4(1.0);

	//Model transformation
	M = glm::rotate(I, base_rotation_angles.y + new_rotation_angles.y, glm::vec3(1.0, 0.0, 0.0));
	M = glm::rotate(M, base_rotation_angles.x + new_rotation_angles.x, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(u_M_location, 1, GL_FALSE, glm::value_ptr(M));

	//View transformation
	if (projection_type) {
		eye = glm::vec3(0.0, 0.0, -2.0 * world_radious);
		at = glm::vec3(0.0, 0.0, 0.0);
		up = glm::vec3(0.0, 1.0, 0.0);
		V = glm::lookAt(eye, at, up);
	} else {
		V = glm::scale(V, glm::vec3(scale_ortho, scale_ortho, scale_ortho));
	}
	if (scene_rotation_flag) {
		V = glm::rotate(V, scene_rotation_angle, glm::vec3(0.0, 1.0, 0.0));
	}
	glUniformMatrix4fv(u_V_location, 1, GL_FALSE, glm::value_ptr(V));

	//Projection transformation
	if (projection_type) {
		P = glm::perspective(scale_perspective, aspect_ratio, 1.0 * world_radious, 3.0 * world_radious);
	} else {
		double z_distance = scale_ortho > 1.0 ? scale_ortho * world_radious : world_radious;
		double x_distance = window_width >= window_height ? world_radious * aspect_ratio: world_radious;
		double y_distance = window_width >= window_height ? world_radious : world_radious /  aspect_ratio;
		P = glm::ortho(-x_distance, x_distance, -y_distance, y_distance, z_distance, -z_distance);
	}
	glUniformMatrix4fv(u_P_location, 1, GL_FALSE, glm::value_ptr(P));
	


	//Pass texture option to shader
	glUniform1i(texture_option_loc, texture_mapping_flag ? 1 : 0);
	
	scene_creation_ptr->pre_render(a_position_loc, a_normal_loc, a_text_coordinates_loc);
	glDisable(GL_BLEND);
	//Color to floor color
	glm::vec4 color = glm::vec4(0.5, 0.5, 0.0, 1.0);
	//Pass color to shader
	glUniform4fv(u_color_location, 1, glm::value_ptr(color));
	texture_stone_ptr->bind();
	scene_creation_ptr->render_quad(1);	
	//First row color
	color = glm::vec4(0.0, 1.0, 0.0, 1.0);
	glUniform4fv(u_color_location, 1, glm::value_ptr(color));
	scene_creation_ptr->render_quad(2);
	scene_creation_ptr->render_quad(3);
	scene_creation_ptr->render_quad(4);

	if (display_mode == OFF) {
		glDisable(GL_BLEND);
	} else if (display_mode == ALPHA_BLENDING) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
	} else { //display mode additive
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
	}
	//Second row color
	texture_unam_ptr->bind();
	color = glm::vec4(0.0, 0.0, 1.0, 0.5);
	glUniform4fv(u_color_location, 1, glm::value_ptr(color));
	scene_creation_ptr->render_quad(5);
	scene_creation_ptr->render_quad(6);
	scene_creation_ptr->render_quad(7);

	//Third row color
	color = glm::vec4(1.0, 0.0, 0.0, 0.5);
	glUniform4fv(u_color_location, 1, glm::value_ptr(color));
	scene_creation_ptr->render_quad(8);
	scene_creation_ptr->render_quad(9);
	scene_creation_ptr->render_quad(10);
	
	
	scene_creation_ptr->post_render(a_position_loc, a_normal_loc, a_text_coordinates_loc);
	
	glutSwapBuffers();
	
	/*Cleaning for next frame render*/
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glUseProgram(0);
	gl_error();
}


int main (int argc, char* argv[]) {

	
}

void init_program() {
	seconds = 0.0;
	window_width = window_height = 512;
	world_radious = sqrtf(3.0f);
	projection_type = true;
	scene_rotation_angle = 0.0f;
	display_mode = OFF;
	texture_mapping_flag = false;
	reset_camera();
}

void reset_camera() {
	base_rotation_angles = glm::vec2(0.0, 0.0);
	new_rotation_angles = glm::vec2(0.0, 0.0);
	scale_perspective = PI / 8.0f;
	scale_ortho = 1.0;
	scene_rotation_angle = 0.0;
}

void create_glut_window() {
	glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize (512, 512);
	win = glutCreateWindow ("Jorge Garcia, Project 4");
}

void create_glut_callbacks() {
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keyboard);
	glutMouseWheelFunc(mouse_wheel);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_active);
	glutReshapeFunc(reshape);
}

void exit_glut() {
	delete gl_program_ptr;
	delete texture_stone_ptr;
	delete scene_creation_ptr;
	
	glutDestroyWindow(win);
	exit(EXIT_SUCCESS);
}

void reshape(int new_window_width, int new_window_height) {
	window_width = new_window_width == 0 ? 1 : new_window_width;
	window_height = new_window_height == 0 ? 1 : new_window_height;
	aspect_ratio = static_cast<double>(window_width) / static_cast<double>(window_height);

	glViewport(0, 0, window_width, window_height);
}

void special_keyboard(int key, int mouse_x, int mouse_y) {
	
	switch (key) {
		case GLUT_KEY_F1:
			scene_rotation_flag = !scene_rotation_flag;
			reset_camera();
		break;

		case GLUT_KEY_F2:
			if (display_mode ==  ALPHA_BLENDING) {
				display_mode = OFF;
			} else {
				display_mode = ALPHA_BLENDING;
			}
		break;

		case GLUT_KEY_F3:
			if (display_mode ==  ADDITIVE_BLENDING) {
				display_mode = OFF;
			} else {
				display_mode = ADDITIVE_BLENDING;
			}
		break;

		case GLUT_KEY_F4:
			texture_mapping_flag = !texture_mapping_flag;
		break;
	}

	glutPostRedisplay();
}

void keyboard(unsigned char key, int mouse_x, int mouse_y) {
	switch (key) {

		case 'q':
		case 'Q':
		case 27:
			exit_glut();
		break;

		case 'p':
		case 'P':
			projection_type = !projection_type;
			reset_camera();
		break;

		case 't':
		case 'T':
			texture_mapping_flag = !texture_mapping_flag;	
		break;

	} 
	glutPostRedisplay();
}

void idle() {  
   //timers for time-based animation
   static int last_time = 0;
   int time = glutGet(GLUT_ELAPSED_TIME);
   int elapsed = time - last_time;
   float delta_seconds = 0.001f * elapsed;
   last_time = time;

   scene_rotation_angle += PI * delta_seconds;
   
   
   gl_error(); //check for errors and print error strings
   glutPostRedisplay();
}

void mouse_active(int mouse_x, int mouse_y) {
	glm::vec2 mouse_current;
	if (mouse_dragging) {
		mouse_current.x = static_cast<float>(mouse_x);
		mouse_current.y = static_cast<float>(mouse_y);
	
		glm::vec2 deltas = mouse_start_drag - mouse_current;

		new_rotation_angles.x = deltas.x / glutGet(GLUT_WINDOW_WIDTH) * PI;
		new_rotation_angles.y = deltas.y / glutGet(GLUT_WINDOW_HEIGHT) * PI;
	}
	glutPostRedisplay();
}

void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y) {
	const float SECTIONS = PI / 20.0f;
	if (projection_type) {
		if (direction > 0 && scale_perspective < (PI - SECTIONS)) {
			scale_perspective += SECTIONS;
		} else if (direction < 0 && scale_perspective > SECTIONS) {
			scale_perspective -= SECTIONS;
		}
		//std::cout << "Scale: " << scale_perspective << std::endl;
	} else {
		if (direction > 0 && scale_ortho < 64.0) {
			scale_ortho *= 2.0;
		} else if (direction < 0 && scale_ortho > 0.125) {
			scale_ortho /= 2.0;
		}
		//std::cout << "Scale: " << scale_ortho << std::endl;
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int mouse_x, int mouse_y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		mouse_dragging = true;
		mouse_start_drag.x = static_cast<float>(mouse_x);
		mouse_start_drag.y = static_cast<float>(mouse_y);
	} else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		mouse_dragging = false;
		base_rotation_angles += new_rotation_angles;
		new_rotation_angles = glm::vec2(0.0, 0.0);
		//std::cout << "Rotation Angles: (" << base_rotation_angles.x << ", " << base_rotation_angles.y << ")"<< std::endl;
	}
	glutPostRedisplay();
}


