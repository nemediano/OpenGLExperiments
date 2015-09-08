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
// Location for shader variables
GLint u_PVM_location = -1;
GLint a_position_loc = -1;
GLint a_color_loc = -1;
// OpenGL program handlers
GLuint vertex_shader;
GLuint fragment_shader;
GLuint program;

//Global variables for the program logic
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
	window = glutCreateWindow("Hello world OpenGL");
}

void init_program() {
	/* Initialize global variables for program control */
	nTriangles = 1;
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
	const unsigned int nVertex = 3;
	const unsigned int nIndices = 3;
	nTriangles = 1;

	Vertex points[nVertex] = {
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, }, //0
		{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, }, //1
		{ {  0.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } }, //2
	};

	unsigned short indices[nIndices] = { 
		0, 1, 2,
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
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
}

void reshape(int new_window_width, int new_window_height) {
	glViewport(0, 0, new_window_width, new_window_height);
}

void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);

	/************************************************************************/
	/* Calculate  Model View Projection Matrices                            */
	/************************************************************************/
	//Identity matrix
	glm::mat4 I(1.0f);
	//Model
	glm::mat4 M = I;
	//View
	glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 camera_eye = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 V = glm::lookAt(camera_position, camera_eye, camera_up);
	//Projection
	GLfloat aspect = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	GLfloat fovy = TAU / 8.0f;
	GLfloat zNear = 0.01f;
	GLfloat zFar = 10000.0f;
	glm::mat4 P = glm::perspective(fovy, aspect, zNear, zFar);

	/************************************************************************/
	/* Send uniform values to shader                                        */
	/************************************************************************/
	if (u_PVM_location != -1) {
		glUniformMatrix4fv(u_PVM_location, 1, GL_FALSE, glm::value_ptr(P * V * M));
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