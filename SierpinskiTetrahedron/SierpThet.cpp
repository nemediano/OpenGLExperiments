#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>

#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cassert>

#include "opengl/HelperFunctions.h"
#include "opengl/OpenGLProgram.h"

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

const float TAU = 6.28318f;
const float PI = 3.14159f;

static int win = 0;
GLuint vertex_array_object;
GLuint buffer;
GLint vertex_position_loc;
GLint vertex_color_loc;
GLint model_view_loc;
GLint projection_loc;

//Camera handling
glm::quat camera_base_rotation;
glm::quat camera_new_rotation;
float projection_on_curve(glm::vec2 projected);


bool mouse_dragging;
glm::vec2 mouse_start_drag;
GLfloat scale_perspective;
GLfloat scale_ortho;

const GLuint max_recursion_depth = 6;
const GLuint max_number_of_vertex = static_cast<int>(pow(4.0, max_recursion_depth)) * 4 * 3;
GLuint recursion_depth;
GLboolean projection_type;
std::vector<glm::vec3> tet_vertex;
std::vector<glm::vec3> colors;

float seconds;
int window_width;
int window_height;

//OpenGL program object
opengl::OpenGLProgram* gl_program_ptr = nullptr;

//Program logic functions
void create_triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, int color);
void create_fractal();
void subdivide_tetrahedra(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, int recursion_level);
GLuint number_of_vertex;
GLuint number_of_tets;
GLuint number_of_triangles;


//Callback functions for freeglut
void special_keys(int key, int mouse_x, int mouse_y);
void keyboard(unsigned char key, int mouse_x, int mouse_y);
void mouse_active(int mouse_x, int mouse_y);
void mouse(int button, int state, int mouse_x, int mouse_y);
void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y);
void display();
void idle();

//program management
void create_window();
void create_callbacks();
void init_OpenGL();
void exit_program();
void init_program();

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	create_window();
	create_callbacks();

	init_OpenGL();
	init_program();

	glutMainLoop();

	exit_program();
	return EXIT_SUCCESS;
}

void display() {
	using glm::vec3;
	using glm::vec4;
	using glm::mat4;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
	mat4 M = mat4(1.0);
	mat4 V = mat4(1.0);
	mat4 Proj = mat4(1.0);

	//Model transformation
	M = glm::mat4_cast(camera_new_rotation * camera_base_rotation);

	//View transformation
	float world_radious = 1.0f;
	if (projection_type) {
		vec3 eye = vec3(0.0, 0.0, -2.0 * world_radious);
		vec3 at = vec3(0.0, 0.0, 0.0);
		vec3 up = vec3(0.0, 1.0, 0.0);
		V = glm::lookAt(eye, at, up);
	}
	else {
		V = glm::scale(M, vec3(scale_ortho, scale_ortho, scale_ortho));
	}

	//Projection transformation
	if (projection_type) {
		GLfloat aspect = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
		Proj = glm::perspective(scale_perspective, aspect, 1.0f * world_radious, 3.0f * world_radious);
	}
	else {
		double z_distance = scale_ortho > 1.0 ? scale_ortho : 1.0;
		Proj = glm::ortho(-1.0, 1.0, -1.0, 1.0, z_distance, -z_distance);
	}
	//Pass uniform variables to shaders
	if (model_view_loc != -1) {
		glUniformMatrix4fv(model_view_loc, 1, GL_FALSE, glm::value_ptr(V * M));
	}
	if (projection_loc != -1) {
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(Proj));
	}
	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, number_of_vertex);
	glutSwapBuffers();
}

void create_fractal() {
	//Specify the initial vertices for a tetrahedron
	glm::vec3 vertices[4] = {
		glm::vec3(0.0, 0.0, -1.0),
		glm::vec3(0.0, (2.0 / 3.0) * sqrt(2.0), 1.0 / 3.0),
		glm::vec3(-sqrt(2.0 / 3.0), -sqrt(2.0) / 3.0, 1.0 / 3.0),
		glm::vec3(sqrt(2.0 / 3.0), -sqrt(2.0) / 3.0, 1.0 / 3.0)
	};

	tet_vertex.clear();
	colors.clear();
	subdivide_tetrahedra(vertices[0], vertices[1], vertices[2], vertices[3], recursion_depth);
	assert(tet_vertex.size() == colors.size());
	number_of_tets = static_cast<int>(pow(4.0, recursion_depth));
	number_of_triangles = number_of_tets * 4;
	number_of_vertex = number_of_triangles * 3;

	//Send generated data to GPU
	//The vertex
	glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_vertex * sizeof(glm::vec3), &tet_vertex[0]);
	//Leave an offset of half buffer, then write the colors
	glBufferSubData(GL_ARRAY_BUFFER, max_number_of_vertex * sizeof(glm::vec3), number_of_vertex * sizeof(glm::vec3), &colors[0]);
}

void subdivide_tetrahedra(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, int recursion_level) {
	if (recursion_level > 0) {
		glm::vec3 mid_points[6];

		mid_points[0] = (a + b) / 2.0f;
		mid_points[1] = (a + c) / 2.0f;
		mid_points[2] = (a + d) / 2.0f;
		mid_points[3] = (b + c) / 2.0f;
		mid_points[4] = (b + d) / 2.0f;
		mid_points[5] = (d + c) / 2.0f;

		subdivide_tetrahedra(a, mid_points[0], mid_points[1], mid_points[2], recursion_level - 1);
		subdivide_tetrahedra(mid_points[0], b, mid_points[3], mid_points[4], recursion_level - 1);
		subdivide_tetrahedra(mid_points[2], mid_points[4], mid_points[5], d, recursion_level - 1);
		subdivide_tetrahedra(mid_points[1], mid_points[3], c, mid_points[5], recursion_level - 1);

	} else {
		create_triangle(d, b, c, 3);
		create_triangle(a, b, d, 1);
		create_triangle(c, a, b, 2);
		create_triangle(a, d, c, 0);
	}
}

void create_triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, int color) {
	glm::vec3 color_pallete[4] = {
		glm::vec3(1.0, 0.0, 0.0), //Red
		glm::vec3(0.0, 1.0, 0.0), //Green
		glm::vec3(0.0, 0.0, 1.0), //Blue
		glm::vec3(1.0, 1.0, 0.0)  //Yellow
	};

	assert(color >= 0 && color < 4);
	tet_vertex.push_back(a);
	colors.push_back(color_pallete[color]);
	tet_vertex.push_back(b);
	colors.push_back(color_pallete[color]);
	tet_vertex.push_back(c);
	colors.push_back(color_pallete[color]);
}

void init_OpenGL() {
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
	}
	cout << opengl::get_OpenGL_info() << endl;
	//Register OpenGL error log if we are in debug mode
	opengl::get_error_log();

	// Create and bind a vertex array object
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);

	// Create and initialize a buffer object
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	unsigned int buffer_size = max_number_of_vertex * 2 * sizeof(glm::vec3);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);

	//create and load shaders
	gl_program_ptr = new opengl::OpenGLProgram("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
	gl_program_ptr->use_program();

	// Get location to uniform variables
	model_view_loc = gl_program_ptr->get_uniform_location("VM");
	projection_loc = gl_program_ptr->get_uniform_location("Proj");

	// Initialize the vertex position attribute from the vertex shader
	vertex_position_loc = gl_program_ptr->get_attrib_location("vPosition");
	glEnableVertexAttribArray(vertex_position_loc);
	glVertexAttribPointer(vertex_position_loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	// Initialize the vertex color
	vertex_color_loc = gl_program_ptr->get_attrib_location("vColor");
	glEnableVertexAttribArray(vertex_color_loc);
	//Leave an offset of half buffer, then put colors there
	unsigned int offset = max_number_of_vertex * sizeof(glm::vec3);
	glVertexAttribPointer(vertex_color_loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));

	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void init_program() {
	seconds = 0.0;
	camera_base_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	camera_new_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	window_width = window_height = 640;
	recursion_depth = 0;
	projection_type = false;
	scale_perspective = TAU / 8.0f;
	scale_ortho = 1.0;
	create_fractal();
}

void create_window() {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(5, 5);
	glutInitWindowSize(640, 640);
	win = glutCreateWindow("Sierpinski Pyramid");
}

void create_callbacks() {
	glutDisplayFunc(display);
	glutSpecialFunc(special_keys);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouse_wheel);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_active);
}

void exit_program() {
	if (gl_program_ptr) {
		delete gl_program_ptr;
		gl_program_ptr = nullptr;
	}
	glDeleteBuffers(1, &buffer);
	glDeleteVertexArrays(1, &vertex_array_object);
	glutDestroyWindow(win);
}



void idle() {
	static int last_time = 0;

	int time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = time - last_time;
	last_time = time;
	float delta_seconds = 0.001f * elapsed;
	seconds += delta_seconds;

	//set shader time uniform variable
	gl_program_ptr->use_program();

	/*if(Time_loc != -1) {
	glUniform1f(Time_loc, seconds);
	}*/

	glutPostRedisplay();
}

void special_keys(int key, int mouse_x, int mouse_y) {
	switch (key) {
	case GLUT_KEY_UP:
		if (recursion_depth < max_recursion_depth) {
			recursion_depth++;
			create_fractal();
		}
		break;

	case GLUT_KEY_DOWN:
		if (recursion_depth > 0) {
			recursion_depth--;
			create_fractal();
		}
		break;
	}
	std::cout << "Recursion depth: " << recursion_depth << std::endl;
	glutPostRedisplay();
}

void keyboard(unsigned char key, int mouse_x, int mouse_y) {
	switch (key) {
	case 'q':
	case 'Q':
	case 27:
		exit_program();
		exit(EXIT_SUCCESS);
		break;

	case 'R':
	case 'r':
		camera_base_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
		scale_perspective = TAU / 8.0f;
		scale_ortho = 1.0;
	default:
		break;

	case 'p':
	case 'P':
		projection_type = (!projection_type);
		break;
	}

	glutPostRedisplay();
}


void mouse_active(int mouse_x, int mouse_y) {
	glm::vec2 mouse_current;
	if (mouse_dragging) {
		mouse_current = glm::vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
		glm::vec2 scale_factors = glm::vec2(2.0f / glutGet(GLUT_WINDOW_WIDTH), -2.0f / glutGet(GLUT_WINDOW_HEIGHT));

		glm::vec2 window_center = 0.5f * glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		glm::vec2 mouse_current_in_world = scale_factors * (mouse_current - window_center);
		glm::vec2 mouse_start_drag_in_world = scale_factors * (mouse_start_drag - window_center);

		glm::vec3 v_1 = glm::vec3(mouse_current_in_world, projection_on_curve(mouse_current_in_world));
		glm::vec3 v_2 = glm::vec3(mouse_start_drag_in_world, projection_on_curve(mouse_start_drag_in_world));
		v_1 = glm::normalize(v_1);
		v_2 = glm::normalize(v_2);
		glm::vec3 axis = glm::cross(v_1, v_2);
		GLfloat angle = glm::angle(v_1, v_2);
		camera_new_rotation = glm::normalize(glm::quat(glm::cos(0.5f * angle), glm::sin(0.5f * angle) * axis));
	}

	glutPostRedisplay();
}

float projection_on_curve(glm::vec2 projected) {
	const float radius = 0.5f;
	float z = 0.0f;
	if (glm::length2(projected) <= (0.5f * radius * radius)) {
		//Inside the sphere
		z = glm::sqrt(radius * radius - glm::length2(projected));
	}
	else {
		//Outside of the sphere using hyperbolic sheet
		z = (0.5f * radius * radius) / glm::length(projected);
	}
	return z;
}

void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y) {
	const float DELTA_ANGLE = PI / 20.0f;

	if (projection_type) {
		if (direction > 0 && scale_perspective < (PI - 2.0f * DELTA_ANGLE)) {
			scale_perspective += DELTA_ANGLE;
		}
		else if (scale_perspective > 2.0 * DELTA_ANGLE){
			scale_perspective -= DELTA_ANGLE;
		}
		
	} else {
		if (direction > 0 && scale_ortho < 64.0) {
			scale_ortho *= 2.0;
		}
		else if (direction < 0 && scale_ortho > 0.125) {
			scale_ortho /= 2.0;
		}
		
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int mouse_x, int mouse_y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		mouse_dragging = true;
		mouse_start_drag = glm::vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
	} else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		mouse_dragging = false;
		/* Calculate the accumulated rotation: base rotation plus new one */
		camera_base_rotation = glm::normalize(camera_new_rotation * camera_base_rotation);
		/* Reset new rotation to identity */
		camera_new_rotation = glm::normalize(glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f)));
	}
	glutPostRedisplay();
}
