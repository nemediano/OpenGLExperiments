#include <cstdlib>
#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GL/freeglut.h>

#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
#define OFFSET_OF(type, member) ((GLvoid*)(offsetof(type, member)))

//Math constant equal two PI
const float TAU = 6.28318f;

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
};

GLint window = 0;
//Location for shader variables
GLint u_PVM_location = -1;
GLint a_position_loc = -1;
GLint a_color_loc = -1;
//OpenGL program handlers
GLuint vertex_shader;
GLuint fragment_shader;
GLuint program;

// Quaternion trackball related
glm::quat camera_base_rotation;
glm::quat camera_new_rotation;
float projection_on_curve(glm::vec2 projected);
//For the mouse dragging 
bool mouse_dragging;
glm::vec2 mouse_start_drag;


// Global variables for the program logic
int nTriangles;
//Manage the Vertex Buffer Object
GLuint vbo;
GLuint indexBuffer;

void create_glut_window();
void init_program();
void init_OpenGL();
void create_primitives();
void create_glut_callbacks();
void exit_glut();

//Glut callback functions
void display();
void reshape(int new_window_width, int new_window_height);
// To rotate the camera
void mouse_active(int mouse_x, int mouse_y);
void mouse(int button, int state, int mouse_x, int mouse_y);

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	create_glut_window();
	init_OpenGL();
	init_program();

	create_glut_callbacks();
	glutMainLoop();

	return EXIT_SUCCESS;
}

void exit_glut() {
	/* Delete OpenGL program */
	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(program);
	/* Delete window (freeglut) */
	glutDestroyWindow(window);
	exit(EXIT_SUCCESS);
}

void create_glut_window() {
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	window = glutCreateWindow("Simplest trackball rotation");
}

void init_program() {
	mouse_dragging = false;
	/* Initialize global variables for program control */
	nTriangles = 4;
	/* Then, create primitives */
	create_primitives();
}

void init_OpenGL() {
	using std::cout;
	using std::cerr;
	using std::endl;
	/************************************************************************/
	/*                    Init OpenGL context                               */
	/************************************************************************/
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cerr << "Error: " << glewGetErrorString(err) << endl;
	}
	cout << "Hardware specification: " << endl;
	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Software specification: " << endl;
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	cout << "Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Using GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	int ver = glutGet(GLUT_VERSION);
	cout << "Using freeglut version: " << ver / 10000 << "." << (ver / 100) % 100 << "." << ver % 100 << endl;

	/************************************************************************/
	/*                   OpenGL program creation                            */
	/************************************************************************/
	using std::string;

	/* In a normal program the shader should be in separate text files
	I put them here to avoid another layer of complexity */
	string vertex_shader_src =
		"#version 330\n"
		"in vec3 Position;\n"
		"in vec3 Color\n;\n"
		"\n"
		"uniform mat4 PVM;\n"
		"\n"
		"out vec4 vColor;\n"
		"\n"
		"void main(void) {\n"
		"\tgl_Position = PVM * vec4(Position, 1.0f);\n"
		"\tvColor = vec4(Color, 1.0);\n"
		"}\n";

	string fragment_shader_src =
		"#version 330\n"
		"\n"
		"in vec4 vColor;\n"
		"\n"
		"out vec4 fragcolor;\n"
		"\n"
		"void main(void) {\n"
		"\tfragcolor = vColor;\n"
		"}\n";

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	const char* start = &vertex_shader_src[0];
	glShaderSource(vertex_shader, 1, &start, nullptr);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	start = &fragment_shader_src[0];
	glShaderSource(fragment_shader, 1, &start, nullptr);

	int status;
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		cerr << "Vertex shader was not compiled!!" << endl;
	}
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		cerr << "Fragment shader was not compiled!!" << endl;
	}
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		cerr << "OpenGL program was not linked!!" << endl;
	}

	/************************************************************************/
	/* Allocating variables for shaders                                     */
	/************************************************************************/

	u_PVM_location = glGetUniformLocation(program, "PVM");
	a_position_loc = glGetAttribLocation(program, "Position");
	a_color_loc = glGetAttribLocation(program, "Color");

	//Activate anti-alias
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	//Initialize some basic rendering state
	glEnable(GL_DEPTH_TEST);
	//Dark gray background color
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

}

void create_primitives() {

	const unsigned int nVertex = 4;
	const unsigned int nIndices = 12;

	Vertex points[nVertex] = {
		{ {0.0f, 0.0f, 1.0f},                                         {1.0f, 1.0f, 0.0f} },
		{ {0.0, (2.0 / 3.0) * glm::sqrt(2.0), -1.0 / 3.0},            {1.0f, 0.0f, 0.0f} },
		{ {-glm::sqrt(2.0 / 3.0), -glm::sqrt(2.0) / 3.0, -1.0 / 3.0}, {0.0f, 0.0f, 1.0f} },
		{ {glm::sqrt(2.0 / 3.0), -glm::sqrt(2.0) / 3.0, -1.0 / 3.0},  {0.0f, 1.0f, 0.0f} },
	};

	unsigned short indices[nIndices] = {
		1, 2, 3,
		0, 1, 3,
		0, 1, 2,
		0, 3, 2,
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned short), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void create_glut_callbacks() {
	glutMotionFunc(mouse_active);
	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
}

void reshape(int new_window_width, int new_window_height) {
	glViewport(0, 0, new_window_width, new_window_height);
}

void display() {
	using glm::vec3;
	using glm::vec4;
	using glm::mat4;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);

	/************************************************************************/
	/* Calculate  Model View Projection Matrices                            */
	/************************************************************************/
	//Identity matrix
	mat4 I(1.0f);
	//Model
	mat4 M = I;
	//View
	/* 
	GLM can construct the matrix automatically directly using the method described in
	https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Conversion_to_and_from_the_matrix_representation
	*/
	mat4 camRot = glm::mat4_cast(camera_new_rotation * camera_base_rotation);
	vec3 camera_up = vec3(0.0f, 1.0f, 0.0f);
	vec3 camera_position = vec3(0.0f, 0.0f, 3.0f);
	vec3 camera_eye = vec3(0.0f, 0.0f, 0.0f);
	mat4 V = glm::lookAt(camera_position, camera_eye, camera_up);
	//Projection
	GLfloat aspect = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	GLfloat fovy = TAU / 7.0f;
	GLfloat zNear = 0.1f;
	GLfloat zFar = 10.0f;
	mat4 P = glm::perspective(fovy, aspect, zNear, zFar);

	/************************************************************************/
	/* Send uniform values to shader                                        */
	/************************************************************************/
	if (u_PVM_location != -1) {
		glUniformMatrix4fv(u_PVM_location, 1, GL_FALSE, glm::value_ptr(P * V * camRot * M));
	}

	/************************************************************************/
	/* Bind buffer object and their corresponding attributes                */
	/************************************************************************/
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (a_position_loc != -1) {
		glEnableVertexAttribArray(a_position_loc);
		glVertexAttribPointer(a_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(Vertex, position));
	}
	if (a_color_loc != -1) {
		glEnableVertexAttribArray(a_color_loc);
		glVertexAttribPointer(a_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(Vertex, color));
	}
	//Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	/* Draw */
	glDrawElements(GL_TRIANGLES, 3 * nTriangles, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0 * sizeof(unsigned short)));

	/* Unbind and clean */
	if (a_position_loc != -1) {
		glDisableVertexAttribArray(a_position_loc);
	}
	if (a_color_loc != -1) {
		glDisableVertexAttribArray(a_color_loc);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	glutSwapBuffers();
}

/************************************************************************/
/* Trackball related functions                                          */
/************************************************************************/
void mouse_active(int mouse_x, int mouse_y) {
	using glm::vec2;
	using glm::vec3;

	vec2 mouse_current;
	if (mouse_dragging) {
		mouse_current = vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
		/*
		At this point mouse_start_drag and mouse_current are in pixel coordinates (device)
		we need to transform them in world coordinates, in order to do that we need to do a two step process:
		1.- Translating to the scene center (Mouse coordinates are not in the center of the window but
		rather in the upper left corner of the window).
		2.- Scale to the same coordinate system, remember they are in pixel, not in the [-1, 1] x [-1, 1]
		that we are after projection.
		3.- Invert the Y coordinate since in most window systems the pixel coordinates are reversed. I. e.
		positive direction is down, not up.
		*/
		vec2 window_center = 0.5f * vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		vec2 scale_factors = vec2(2.0f / glutGet(GLUT_WINDOW_WIDTH), -2.0f / glutGet(GLUT_WINDOW_HEIGHT));
		vec2 mouse_current_in_world = scale_factors * (mouse_current - window_center);
		vec2 mouse_start_drag_in_world = scale_factors * (mouse_start_drag - window_center);
		/* Update the new rotation using the algorithm described in https://www.opengl.org/wiki/Trackball */
		vec3 v_1 = glm::normalize(vec3(mouse_current_in_world, projection_on_curve(mouse_current_in_world)));
		vec3 v_2 = glm::normalize(vec3(mouse_start_drag_in_world, projection_on_curve(mouse_start_drag_in_world)));
		glm::vec3 axis = glm::cross(v_1, v_2);
		float angle = glm::angle(v_1, v_2);
		camera_new_rotation = glm::normalize(glm::quat(glm::cos(angle * 0.5f), glm::sin(angle * 0.5f) * axis));
	}
	glutPostRedisplay();
}
/*
 Which curve you use affect the numerical stability of the algorithm.
 This is why most of the people don't actually uses a sphere but rather a 
 the hyperbolic sheet described in:
 https://www.opengl.org/wiki/Object_Mouse_Trackball
*/
float projection_on_curve(glm::vec2 projected) {
	//This is the distance where the curves changed in terms of the window size
	const float radius = 0.8f;
	float z = 0.0f;
	if (glm::length2(projected) <= (0.5f * radius * radius)) {
		//Inside the sphere
		z = glm::sqrt(radius * radius - glm::length2(projected));
	} else {
		//Outside of the sphere using hyperbolic sheet
		z = (0.5f * radius * radius) / glm::length(projected);
	}
	return z;
}

void mouse(int button, int state, int mouse_x, int mouse_y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouse_dragging = true;
			mouse_start_drag.x = static_cast<float>(mouse_x);
			mouse_start_drag.y = static_cast<float>(mouse_y);
		} else {
			mouse_dragging = false;
			/* Calculate the accumulated rotation: base rotation plus new one */
			camera_base_rotation = glm::normalize(camera_new_rotation * camera_base_rotation);
			/* Reset new rotation to identity */
			camera_new_rotation = glm::normalize(glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f)));
		}
	}
	glutPostRedisplay();
}