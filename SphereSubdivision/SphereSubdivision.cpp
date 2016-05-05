#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>

#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl/HelperFunctions.h"
#include "opengl/OpenGLProgram.h"
#include "glut/Globals.h"
#include "glut/Callbacks.h"
#include "scene/Light.h"
#include "scene/Material.h"

#include "SphereSubdivision.h"

using namespace std;

std::vector<Vertex> vertices;
std::vector<unsigned short> indices;

struct Triangle {
	glm::vec3 p_0;
	glm::vec3 p_1;
	glm::vec3 p_2;
};

std::vector<Triangle> triangles;

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	create_glut_window();

	init_OpenGL();
	init_program();

	create_sphere();

	create_glut_callbacks();
	glutMainLoop();

	return EXIT_SUCCESS;
}


void exit_glut() {
	if (options::vbo != 0 || options::indexBuffer != 0) {
		glDeleteBuffers(1, &options::vbo);
		glDeleteBuffers(1, &options::indexBuffer);
	}
	delete options::program_ptr;
	glutDestroyWindow(options::window);
	exit(EXIT_SUCCESS);
}

void init_OpenGL() {
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
	}
	opengl::get_OpenGL_info();

	options::program_ptr = new opengl::OpenGLProgram("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");

	if (!options::program_ptr->is_ok()) {
		cerr << "Error at GL program creation" << endl;
		opengl::gl_error();
		exit(EXIT_FAILURE);
	}

	opengl::get_error_log();

	options::u_PVM_location = options::program_ptr->get_uniform_location("PVM");
	options::u_NormalMatrix_location = options::program_ptr->get_uniform_location("NormalMatrix");
	options::u_VM_location = options::program_ptr->get_uniform_location("VM");

	options::a_position_loc = options::program_ptr->get_attrib_location("Position");
	options::a_normal_loc = options::program_ptr->get_attrib_location("Normal");

	//Light options for the fragment shader
	options::u_LightPosition_location = options::program_ptr->get_uniform_location("lightPosition");
	options::u_La_location = options::program_ptr->get_uniform_location("La");
	options::u_Ld_location = options::program_ptr->get_uniform_location("Ld");
	options::u_Ls_location = options::program_ptr->get_uniform_location("Ls");
	options::u_view = options::program_ptr->get_uniform_location("view");

	//Material properties for the fragment shader
	options::u_Ka_location = options::program_ptr->get_uniform_location("Ka");
	options::u_Kd_location = options::program_ptr->get_uniform_location("Kd");
	options::u_Ks_location = options::program_ptr->get_uniform_location("Ks");
	options::u_shininess_location = options::program_ptr->get_uniform_location("shininess");

	//Activate antialliasing
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);
	
	//initialize some basic rendering state
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.85f, 0.85f, 0.85f, 1.0f);

	opengl::gl_error("At scene creation");
}

void create_glut_window() {
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 600);
	options::window = glutCreateWindow("Sphere subdivision");
}

void create_glut_callbacks() {
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouse_wheel);
	glutSpecialFunc(special_keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_active);
	glutReshapeFunc(reshape);
}



void init_program() {
	reset_camera();
	//Create light source
	options::light.setType(scene::PUNTUAL);
	//Remember shader calculations are in view space, this light is always a little above the
	//camera. (These coordinates are relative to the camera center).
	//Angle that makes the camera with the center
	const float light_angle = TAU / 16.0f;
	options::light.setPosition(options::world_radious * glm::vec3(0.0f, glm::sin(light_angle), 1.0f - cos(light_angle)));
	options::light.setDirection(glm::vec3(0.0f));
	options::light.setAperture(TAU / 8.0f);

	//Create material
	glm::vec3 Ka = glm::vec3(0.0215f, 0.1745f, 0.0215f);
	glm::vec3 Kd = glm::vec3(0.07568f, 0.61424f, 0.07568f);
	glm::vec3 Ks = glm::vec3(0.633f, 0.727811f, 0.633f);
	float shine = 10.0f;
	options::material = scene::Material(Ka, Kd, Ks, shine);
}


void display() {

	using glm::vec3;
	using glm::vec4;
	using glm::mat4;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	options::program_ptr->use_program();
	mat4 I(1.0f);

	//Model
	mat4 M = glm::scale(I, glm::vec3(1.0f, 1.0f, 1.0f));

	//View
	/* Camera rotation must be accumulated: base rotation then new rotation */
	mat4 camRot = glm::mat4_cast(options::camera_new_rotation * options::camera_base_rotation);
	vec3 position = options::camera_position;
	vec3 center = options::camera_center;
	mat4 V = glm::lookAt(position, center, options::camera_up) * camRot;

	//Projection
	GLfloat aspect = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	GLfloat fovy = options::field_of_view_y;
	GLfloat zNear = 0.1f;
	GLfloat zFar = 100.0f;
	mat4 P = glm::perspective(fovy, aspect, zNear, zFar);

	vec4 color = vec4(1.0f, 1.0f, 0.0f, 1.0f);

	if (options::u_PVM_location != -1) {
		glUniformMatrix4fv(options::u_PVM_location, 1, GL_FALSE, glm::value_ptr(P * V * M));
	}
	if (options::u_VM_location != -1) {
		glUniformMatrix4fv(options::u_VM_location, 1, GL_FALSE, glm::value_ptr(V * M));
	}
	if (options::u_NormalMatrix_location != -1) {
		glUniformMatrix4fv(options::u_NormalMatrix_location, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(V * M))));
	}

	//Pass light source to shader
	pass_light_and_material();

	/* Bind */
	glBindBuffer(GL_ARRAY_BUFFER, options::vbo);
	if (options::a_position_loc != -1) {
		glEnableVertexAttribArray(options::a_position_loc);
		glVertexAttribPointer(options::a_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(Vertex, position));
	}
	if (options::a_normal_loc != -1) {
		glEnableVertexAttribArray(options::a_normal_loc);
		glVertexAttribPointer(options::a_normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(Vertex, normal));
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, options::indexBuffer);

	/* Draw */
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

	/* Unbind and clean */
	if (options::a_position_loc != -1) {
		glDisableVertexAttribArray(options::a_position_loc);
	}
	if (options::a_normal_loc != -1) {
		glDisableVertexAttribArray(options::a_normal_loc);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);


	glutSwapBuffers();
	opengl::gl_error("At the end of display");
}


void pass_light_and_material() {
	//Light properties
	if (options::u_LightPosition_location != -1) {
		glUniform3fv(options::u_LightPosition_location, 1, glm::value_ptr(options::light.getPosition()));
	}
	if (options::u_La_location != -1) {
		glUniform3fv(options::u_La_location, 1, glm::value_ptr(options::light.getLa()));
	}
	if (options::u_Ld_location != -1) {
		glUniform3fv(options::u_Ld_location, 1, glm::value_ptr(options::light.getLd()));
	}
	if (options::u_Ls_location != -1) {
		glUniform3fv(options::u_Ls_location, 1, glm::value_ptr(options::light.getLs()));
	}

	//Material properties
	if (options::u_Ka_location != -1) {
		glUniform3fv(options::u_Ka_location, 1, glm::value_ptr(options::material.getKa()));
	}
	if (options::u_Kd_location != -1) {
		glUniform3fv(options::u_Kd_location, 1, glm::value_ptr(options::material.getKd()));
	}
	if (options::u_Ks_location != -1) {
		glUniform3fv(options::u_Ks_location, 1, glm::value_ptr(options::material.getKs()));
	}
	if (options::u_shininess_location != -1) {
		glUniform1f(options::u_shininess_location, options::material.getShininnes());
	}
}

void create_sphere() {
	triangles.clear();
	
	if (options::icosahedron) {
		start_icosahedra();
	} else {
		start_thetrahedra();
	}
	
	vertices.clear();
	indices.clear();
	create_indexed_mesh();

	//Create the buffers
	if (options::vbo != 0 || options::indexBuffer != 0) {
		glDeleteBuffers(1, &options::vbo);
		glDeleteBuffers(1, &options::indexBuffer);
	}
	glGenBuffers(1, &options::vbo);
	glGenBuffers(1, &options::indexBuffer);

	//Send data to GPU
	//First send the vertices
	glBindBuffer(GL_ARRAY_BUFFER, options::vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Now, the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, options::indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void start_thetrahedra() {
	// Create first four vertex of a tetrahedron
	glm::vec3 p_0 = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 p_1 = glm::vec3(0.0, (2.0 / 3.0) * glm::sqrt(2.0), -1.0 / 3.0);
	glm::vec3 p_2 = glm::vec3(-glm::sqrt(2.0 / 3.0), -glm::sqrt(2.0) / 3.0, -1.0 / 3.0);
	glm::vec3 p_3 = glm::vec3(glm::sqrt(2.0 / 3.0), -glm::sqrt(2.0) / 3.0, -1.0 / 3.0);

	//Iterate doing the decomposition
	subdivide_face(p_0, p_2, p_3, options::subdiv_level);
	subdivide_face(p_0, p_3, p_1, options::subdiv_level);
	subdivide_face(p_0, p_1, p_2, options::subdiv_level);
	subdivide_face(p_1, p_2, p_3, options::subdiv_level);
}

void start_icosahedra() {
	using namespace std;
	using glm::vec3;
	//Start creating the 12 original vertices
	vector<vec3> initial_vertices(12, vec3(0.0f));
	//Spherical coordinates
	float phi = 0.0f; //Between [0, and Pi]
	float psy = 0.0f; //Between [0, and Tau]
	float radio = 1.0f;

	//North pole
	initial_vertices[0] = radio * vec3(sin(phi) * cos(psy), sin(phi) * sin(psy), cos(phi));
	
	phi = PI / 3.0f;;
	for (int i = 1; i <= 5; ++i) {
		initial_vertices[i] = radio * vec3(sin(phi) * cos(psy), sin(phi) * sin(psy), cos(phi));
		psy += TAU / 5.0f;
	}

	psy = TAU / 10.0f;
	phi = PI - (PI / 3.0f);
	for (int i = 1; i <= 5; ++i) {
		initial_vertices[i + 5] = radio * vec3(sin(phi) * cos(psy), sin(phi) * sin(psy), cos(phi));
		psy += TAU / 5.0f;
	}

	//South pole
	phi = PI;
	psy = 0.0f;
	initial_vertices[11] = radio * vec3(sin(phi) * cos(psy), sin(phi) * sin(psy), cos(phi));

	//Generate the initial 20 faces
	subdivide_face(initial_vertices[0], initial_vertices[1], initial_vertices[2], options::subdiv_level);
	subdivide_face(initial_vertices[0], initial_vertices[2], initial_vertices[3], options::subdiv_level);
	subdivide_face(initial_vertices[0], initial_vertices[3], initial_vertices[4], options::subdiv_level);
	subdivide_face(initial_vertices[0], initial_vertices[4], initial_vertices[5], options::subdiv_level);
	subdivide_face(initial_vertices[0], initial_vertices[5], initial_vertices[1], options::subdiv_level);
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	subdivide_face(initial_vertices[1], initial_vertices[6], initial_vertices[2], options::subdiv_level);
	subdivide_face(initial_vertices[2], initial_vertices[6], initial_vertices[7], options::subdiv_level);
	subdivide_face(initial_vertices[2], initial_vertices[7], initial_vertices[3], options::subdiv_level);
	subdivide_face(initial_vertices[3], initial_vertices[7], initial_vertices[8], options::subdiv_level);
	subdivide_face(initial_vertices[3], initial_vertices[8], initial_vertices[4], options::subdiv_level);
	subdivide_face(initial_vertices[4], initial_vertices[8], initial_vertices[9], options::subdiv_level);
	subdivide_face(initial_vertices[4], initial_vertices[9], initial_vertices[5], options::subdiv_level);
	subdivide_face(initial_vertices[5], initial_vertices[9], initial_vertices[10], options::subdiv_level);
	subdivide_face(initial_vertices[5], initial_vertices[10], initial_vertices[1], options::subdiv_level);
	subdivide_face(initial_vertices[1], initial_vertices[10], initial_vertices[6], options::subdiv_level);
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	subdivide_face(initial_vertices[6], initial_vertices[11], initial_vertices[7], options::subdiv_level);
	subdivide_face(initial_vertices[7], initial_vertices[11], initial_vertices[8], options::subdiv_level);
	subdivide_face(initial_vertices[8], initial_vertices[11], initial_vertices[9], options::subdiv_level);
	subdivide_face(initial_vertices[9], initial_vertices[11], initial_vertices[10], options::subdiv_level);
	subdivide_face(initial_vertices[10], initial_vertices[11], initial_vertices[6], options::subdiv_level);
}


void subdivide_face(glm::vec3& p_0, glm::vec3& p_1, glm::vec3& p_2, const size_t& level) {

	if (level > 0) {
		//Calculate midpoints and project them to sphere
		glm::vec3 new_01 = glm::normalize(0.5f * (p_0 + p_1));
		glm::vec3 new_12 = glm::normalize(0.5f * (p_1 + p_2));
		glm::vec3 new_20 = glm::normalize(0.5f * (p_2 + p_0));
		//Subdivide again recursively using the new vertex
		subdivide_face(p_0, new_01, new_20, level - 1);
		subdivide_face(new_01, new_12, new_20, level - 1);
		subdivide_face(new_01, p_1, new_12, level - 1);
		subdivide_face(new_20, new_12, p_2, level - 1);
	}
	else {
		//We reach the bottom of the recursion, we need to generate a triangle
		Triangle triangle;
		triangle.p_0 = p_0;
		triangle.p_1 = p_1;
		triangle.p_2 = p_2;
		triangles.push_back(triangle);
	}
}

bool VertexLeesThan(const glm::vec3& lhs, const glm::vec3& rhs) {
	const float EPSILON = 1e-5f;
	if (fabs(lhs.x - rhs.x) > EPSILON) {
		return lhs.x < rhs.x;
	}
	else if (fabs(lhs.y - rhs.y) > EPSILON) {
		return lhs.y < rhs.y;
	}
	else if (fabs(lhs.z - rhs.z) > EPSILON){
		return lhs.z < rhs.z;
	}
	else {
		return false;
	}
}

void create_indexed_mesh() {
	std::set<glm::vec3, bool(*)(const glm::vec3&, const glm::vec3&)> tmp_storage(VertexLeesThan);

	//Insert all the vertex in the tmp_storage
	for (auto triangle : triangles) {
		tmp_storage.insert(triangle.p_0);
		tmp_storage.insert(triangle.p_1);
		tmp_storage.insert(triangle.p_2);
	}

	//Insert index for the vertices
	for (auto triangle : triangles) {
		// P_0
		auto it = tmp_storage.find(triangle.p_0);
		auto index = std::distance(tmp_storage.begin(), it);
		indices.push_back(static_cast<unsigned short>(index));
		// P_1
		it = tmp_storage.find(triangle.p_1);
		index = std::distance(tmp_storage.begin(), it);
		indices.push_back(static_cast<unsigned short>(index));
		// P_2
		it = tmp_storage.find(triangle.p_2);
		index = std::distance(tmp_storage.begin(), it);
		indices.push_back(static_cast<unsigned short>(index));
	}

	//Create the Vertex storage
	Vertex tmp_vertex;
	for (auto position : tmp_storage) {
		tmp_vertex.position = position;
		tmp_vertex.normal = glm::normalize(position);
		vertices.push_back(tmp_vertex);
	}
}