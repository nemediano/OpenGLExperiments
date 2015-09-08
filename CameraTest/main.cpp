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
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "opengl/HelperFunctions.h"
#include "opengl/OpenGLProgram.h"
#include "textures/TextureHandler.h"

opengl::OpenGLProgram* program_ptr = nullptr;
texture::TextureHandler* texture_ptr[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

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
int nTriangles;

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
float projection_on_curve(glm::vec2 projected);


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

	for (int i = 0; i < 6; ++i) {
		delete texture_ptr[i];
	}

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
	
	for (int i = 0; i < 6; ++i) {
		texture_ptr[i] = new texture::TextureHandler();
	}	
	texture_ptr[0]->load_texture(L"img/three.png");
	texture_ptr[1]->load_texture(L"img/one.png");
	texture_ptr[2]->load_texture(L"img/six.png");
	texture_ptr[3]->load_texture(L"img/four.png");
	texture_ptr[4]->load_texture(L"img/two.png");
	texture_ptr[5]->load_texture(L"img/five.png");
	
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
	triangle_rotation = false;
	texture_mapping_flag = true;
	camera_base_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	camera_new_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
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
	glm::mat4 camRot = glm::mat4_cast(glm::normalize(camera_new_rotation) * glm::normalize(camera_base_rotation));
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
	if (u_color_location != -1) {
		glUniform4fv(u_color_location, 1, glm::value_ptr(color));
	}
	
	/* Draw */
	for (int i = 0; i < 6; ++i) {
		texture_ptr[i]->bind();
		glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_SHORT, BUFFER_OFFSET(6 * i * sizeof(unsigned short)));
	}
	
	/* Unbind and clean */
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
	const unsigned int nVertex = 24;
	const unsigned int nIndices = 36;
	nTriangles = 12;

	Vertex points[nVertex] = {
		//Front face of cube
		{ {-1.0f,  1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, }, //0
		{ { 1.0f,  1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, }, //1
		{ {-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, }, //2
		{ { 1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, }, //3
		//Top face of cube
		{ {-1.0f, 1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }, }, //4
		{ { 1.0f, 1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }, },//5
		{ {-1.0f, 1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, },//6
		{ { 1.0f, 1.0f,  1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, }, //7
		//Down face of cube
		{ {-1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, }, //8
		{ { 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, },//9
		{ {-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, },//10
		{ { 1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, }, //11
		//Back face of cube
		{ { 1.0f,  1.0f, -1.0f},  { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f }, }, //12
		{ {-1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f }, }, //13
		{ { 1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f }, }, //14
		{ {-1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f }, },  //15
		//Left face of cube
		{ {-1.0f,  1.0f, -1.0f }, {-1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, }, //16
		{ {-1.0f,  1.0f,  1.0f }, {-1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, }, //17
		{ {-1.0f, -1.0f, -1.0f }, {-1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, }, //18
		{ {-1.0f, -1.0f,  1.0f }, {-1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, },  //19
		//Right face of cube
		{ { 1.0f,  1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }, }, //20
		{ { 1.0f,  1.0f,  1.0f}, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }, }, //21
		{ { 1.0f, -1.0f, -1.0f}, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }, }, //22
		{ { 1.0f, -1.0f,  1.0f}, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, }  //23
	};

	unsigned short indices[nIndices] = { 2, 1, 0, 2, 3, 1, 
		                                 6, 5, 4, 6, 7, 5,
										 10, 9, 8, 10, 11, 9,
										 14, 13, 12, 14, 15, 13,
										 18, 17, 16, 18, 19, 17,
										 22, 21, 20, 22, 23, 21
									    };

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
		mouse_current = glm::vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
		if (mode == PAN) {
			glm::vec2 deltas = mouse_start_drag - mouse_current;
			camera_pan.x = deltas.x / glutGet(GLUT_WINDOW_WIDTH) * 2.0f;
			camera_pan.y = -deltas.y / glutGet(GLUT_WINDOW_HEIGHT) * 2.0f;
		} else if (mode == ROTATION) {
			/* Update the new rotation */
			glm::vec2 window_center = glm::vec2(glutGet(GLUT_WINDOW_WIDTH) * 0.5f, glutGet(GLUT_WINDOW_HEIGHT) * 0.5f);
			glm::vec3 v_1 = glm::vec3(mouse_current - window_center, projection_on_curve(mouse_current));
			glm::vec3 v_2 = glm::vec3(mouse_start_drag - window_center, projection_on_curve(mouse_start_drag));
			v_1 = glm::normalize(v_1);
			v_2 = glm::normalize(v_2);
			glm::vec3 axis = glm::cross(v_1, v_2);
			float angle = glm::angle(v_1, v_2);
			//camera_new_rotation = glm::quat(glm::cos(angle / 0.5f), glm::sin(angle * 0.5f) * axis);
			camera_new_rotation.x = glm::cos(angle * 0.5f);
			camera_new_rotation.y = glm::sin(angle * 0.5f) * axis.x;
			camera_new_rotation.z = glm::sin(angle * 0.5f) * axis.y;
			camera_new_rotation.w = glm::sin(angle * 0.5f) * axis.z;
		}
	}
	glutPostRedisplay();
}

float projection_on_curve(glm::vec2 projected) {
	const float radius = 0.8f;
	float z = 0.0f;
	if (glm::length2(projected) <= (radius * radius * 0.5f)) {
		//Inside the sphere
		z = glm::sqrt(radius * radius - glm::length2(projected));
	} else {
		//Outside of the sphere using hyperbolic sheet
		z = (radius * radius * 0.5f) / glm::length(projected);
	}
	return z;
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
			camera_base_rotation = camera_new_rotation * camera_base_rotation;
			/* Reset new rotation to identity */
			camera_new_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
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

