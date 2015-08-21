#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <iostream>

#include <IL/il.h>
#include <IL/ilu.h>

#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include "opengl/HelperFunctions.h"
#include "opengl/OpenGLProgram.h"
#include "textures/TextureHandler.h"

opengl::OpenGLProgram* program_ptr = nullptr;
texture::TextureHandler* texture_ptr = nullptr;

using namespace std;
//Glut window pointer
int window = 0;

//Variables for GPU side
GLint u_PVM_location = -1;
GLint u_color_location = -1;
GLint u_texture_option_loc = -1;

GLint a_position_loc = -1;
GLint a_normal_loc = -1;
GLint a_textCoord_loc = -1;

//Manage the Vertex Buffer Object
GLuint vbo;
GLuint indexBuffer;

//Two math constants (New glm uses radians as default)
const float TAU = 6.28318f;
const float PI = 3.14159f;
float rotation_angle;
float field_of_view_y;
bool texture_mapping_flag;
bool triangle_rotation = false;

//For the mouse dragging 
bool mouse_dragging;
glm::vec2 mouse_start_drag;
enum CAM_TYPE{ROTATION, PAN, NONE} mode;

//Camera handling
glm::vec3 camera_position;
glm::vec3 camera_center;
glm::vec3 camera_up;
glm::vec2 camera_pan;
glm::quat camera_base_rotation;
glm::quat camera_new_rotation;


//Program management
void exit_glut();
void init_OpenGL();
void init_program();
void create_glut_window();
void create_glut_callbacks();
void reset_camera();

//Scene creation
void create_primitives();

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
#define OFFSET_OF(type, member) ((GLvoid*)(offsetof(type, member)))

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textCoord;
};

//Callback function
void display();
void reshape(int new_window_width, int new_window_height);
//void mouse(int button, int state, int mouse_x, int mouse_y);
void keyboard(unsigned char key, int mouse_x, int mouse_y);
//void special_keyboard(int key, int mouse_x, int mouse_y);
void mouse_active(int mouse_x, int mouse_y);
void mouse(int button, int state, int mouse_x, int mouse_y);
void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y);
void idle();

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	//Init DevIl for texture loading
	ilInit();
	iluInit();

	create_glut_window();
	init_program();
	init_OpenGL();
	//Create scene
	create_primitives();

	create_glut_callbacks();	
	glutMainLoop();

	return EXIT_SUCCESS;
}


void exit_glut() {
	delete program_ptr;
	delete texture_ptr;

	glutDestroyWindow(window);
	exit(EXIT_SUCCESS);
}

void init_OpenGL() {
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
	}
	opengl::get_OpenGL_info();


	program_ptr = new opengl::OpenGLProgram("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
	
	if (!program_ptr->is_ok()) {
		cerr << "Error at GL program creation" << endl;
		opengl::gl_error();
		exit(EXIT_FAILURE);
	}

	opengl::get_error_log();
	
	texture_ptr = new texture::TextureHandler();
	texture_ptr->load_texture(L"img/stone.png");
	
	u_PVM_location = program_ptr->get_uniform_location("PVM");
	u_color_location = program_ptr->get_uniform_location("Color");

	a_position_loc = program_ptr->get_attrib_location("Position");
	a_normal_loc = program_ptr->get_attrib_location("Normal");
	a_textCoord_loc = program_ptr->get_attrib_location("TextCoord");

	//set texture unit for uniform sampler variable
	int tex_loc = program_ptr->get_uniform_location("texture_map");
	u_texture_option_loc = program_ptr->get_uniform_location("texture_option");

	//Activate antialliasing
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	//initialize some basic rendering state
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

	opengl::gl_error("At scene creation");
}

void create_glut_window() {
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	window = glutCreateWindow("Hello world OpenGL");
}

void create_glut_callbacks() {
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouse_wheel);
	/*
	glutSpecialFunc(special_keyboard);
	*/
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_active);
	glutReshapeFunc(reshape);
}

void reshape(int new_window_width, int new_window_height) {
	glViewport(0, 0, new_window_width, new_window_height);
}

void idle() {
	//timers for time-based animation
	static int last_time = 0;
	int time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = time - last_time;
	float delta_seconds = 0.001f * elapsed;
	last_time = time;

	rotation_angle += (PI / 2.0f) * delta_seconds;

	opengl::gl_error("At idle"); //check for errors and print error strings
	glutPostRedisplay();
}

void init_program() {
	mode = NONE;
	mouse_dragging = false;
	rotation_angle = 0.0f;
	triangle_rotation = true;
	texture_mapping_flag = true;
	reset_camera();
}


void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	program_ptr->use_program();
	glm::mat4 I(1.0f);

	/* Rotation must be the accumulated rotation: base plus new */
	//Model
	glm::mat4 M = triangle_rotation ? glm::rotate(I, rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f)) : I;
	//View
	glm::mat4 camRot = glm::mat4_cast(camera_new_rotation) * glm::mat4_cast(camera_base_rotation);
	glm::vec3 position = camera_position + glm::vec3(camera_pan, 0.0f);
	glm::vec3 center = camera_center + glm::vec3(camera_pan, 0.0f);
	glm::mat4 V = glm::lookAt(position, center, camera_up);
	//Projection
	GLfloat aspect = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	GLfloat fovy = field_of_view_y;
	GLfloat zNear = 0.01f;
	GLfloat zFar = 10000.0f;
	glm::mat4 P = glm::perspective(fovy, aspect, zNear, zFar);

	glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

	if (u_PVM_location != -1) {
		glUniformMatrix4fv(u_PVM_location, 1, GL_FALSE, glm::value_ptr(P * V * camRot * M));
	}
	if (u_texture_option_loc) {
		glUniform1i(u_texture_option_loc, texture_mapping_flag ? 1 : 0);
	}
	
	texture_ptr->bind();
	if (u_color_location != -1) {
		glUniform4fv(u_color_location, 1, glm::value_ptr(color));
	}
	
	/*Bind*/
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (a_position_loc != -1) {
		glEnableVertexAttribArray(a_position_loc);
		glVertexAttribPointer(a_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(Vertex, position));
	}
	if (a_normal_loc != -1) {
		glEnableVertexAttribArray(a_normal_loc);
		glVertexAttribPointer(a_normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(Vertex, normal));
	}
	if (a_textCoord_loc != -1) {
		glEnableVertexAttribArray(a_textCoord_loc);
		glVertexAttribPointer(a_textCoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(Vertex, textCoord));
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	/*Draw*/
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
	/*Unbind and clean */
	if (a_position_loc != -1) {
		glDisableVertexAttribArray(a_position_loc);
	}
	if (a_normal_loc != -1) {
		glDisableVertexAttribArray(a_normal_loc);
	}
	if (a_textCoord_loc != -1) {
		glDisableVertexAttribArray(a_textCoord_loc);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	
	glutSwapBuffers();
	opengl::gl_error("At the end of display");
}


void create_primitives() {
	const unsigned int nVertex = 3;
	const unsigned int nIndices = 3;
	
	Vertex points[nVertex] = {
		{ { 0.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}, },
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, },
		{ { 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, }
	};

	unsigned short indices[nIndices] = { 1, 0, 2 };

	//Create the buffers
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &indexBuffer);

	//Send data to GPU
	//First send the vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, nVertex * sizeof(Vertex), points, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Now, the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof (unsigned short), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void keyboard(unsigned char key, int mouse_x, int mouse_y) {
	if (key == 27) {//press ESC to exit
		exit_glut();
	} else if (key == 't' || key == 'T') {
		texture_mapping_flag = (!texture_mapping_flag);
	} else if (key == 'r' || key == 'r') {
		triangle_rotation = (!triangle_rotation);
	} else if (key == 'c' || key == 'C') {
		reset_camera();
	}
}

void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y) {
	const float DELTA_ANGLE = PI / 20.0f;
	if (wheel == 0) {
		if (direction > 0 && field_of_view_y < (PI - 2.0f * DELTA_ANGLE)) {
			field_of_view_y += DELTA_ANGLE;
		} else if (field_of_view_y > 2.0 * DELTA_ANGLE){
			field_of_view_y -= DELTA_ANGLE;
		}
	}
}

void mouse_active(int mouse_x, int mouse_y) {
	glm::vec2 mouse_current;
	if (mouse_dragging) {
		mouse_current.x = static_cast<float>(mouse_x);
		mouse_current.y = static_cast<float>(mouse_y);

		glm::vec2 deltas = mouse_start_drag - mouse_current;
		if (mode == PAN) {
			camera_pan.x = deltas.x / glutGet(GLUT_WINDOW_WIDTH) * 2.0f;
			camera_pan.y = -deltas.y / glutGet(GLUT_WINDOW_HEIGHT) * 2.0f;
		} else if (mode == ROTATION) {
			/*Update the new rotation */
			/*new_rotation_angles.x = deltas.x / glutGet(GLUT_WINDOW_WIDTH) * PI;
			new_rotation_angles.y = deltas.y / glutGet(GLUT_WINDOW_HEIGHT) * PI;*/
		}
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int mouse_x, int mouse_y) {

	if (state == GLUT_DOWN) {
		mouse_dragging = true;
		mouse_start_drag.x = static_cast<float>(mouse_x);
		mouse_start_drag.y = static_cast<float>(mouse_y);
		if (button == GLUT_MIDDLE_BUTTON) {
			mode = PAN;
		} else if (button == GLUT_LEFT_BUTTON) {
			mode = ROTATION;
		}
	} else {
		mouse_dragging = false;
		if (button == GLUT_MIDDLE_BUTTON) {
			camera_center += glm::vec3(camera_pan, 0.0f);
			camera_position += glm::vec3(camera_pan, 0.0f);
			camera_pan = glm::vec2(0.0, 0.0);
		} else if (button == GLUT_LEFT_BUTTON) {
			/* Calculate the accumulated rotation: base rotation plus new one */
			/* Reset the new rotation to identity */
		}
	}
	glutPostRedisplay();
}


void reset_camera() {
	camera_position = glm::vec3(0.0f, 0.0f, 3.5f);
	camera_center = glm::vec3(0.0f, 0.0f, 0.0f);
	camera_up = glm::vec3(0.0f, 1.0f, 0.0);
	field_of_view_y = PI / 4.0f;
}

