#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
#include <cstdlib>
#include <utility>

#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "globals.h"
#include "opengl/OpenGLProgram.h"
#include "opengl/HelperFunctions.h"
#include "callbacks.h"
#include "scene/Flag.h"

//OpenGL vertex buffer objects
GLuint VertexBuffer = 0;
GLuint NormalBuffer = 0;
GLuint TexCoordBuffer = 0;
GLuint IndexBuffer = 0;

using namespace std;
using namespace scene;
static int win = 0;

//OpenGL program object
opengl::OpenGLProgram* gl_program_ptr = nullptr;

void create_glut_window();
void create_glut_callbacks();
void init_open_gl();
void init_program();
void exit_glut();
void bounding_box(glm::vec3* min, glm::vec3* max, float* mesh, int num_vertex);

int main (int argc, char* argv[]) {
	glutInit(&argc, argv);

	create_glut_window();
	create_glut_callbacks();
	init_open_gl();
	init_program();
	
	glutMainLoop();

	exit_glut();
	return EXIT_SUCCESS;		
}

void display() {
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   //Identity matrix to help calculation
   glm::mat4 I(1.0f); 
   
   //Model transformation
   glm::mat4 M = I;

   //View
   glm::mat4 V = I;
   /* Camera rotation must be accumulated: base rotation then new rotation */
   glm::mat4 camRot = glm::mat4_cast(camera_new_rotation * camera_base_rotation);
   glm::vec3 position = camera_position + glm::vec3(camera_pan, 0.0f);
   glm::vec3 center = camera_center + glm::vec3(camera_pan, 0.0f);
   V = glm::lookAt(position, center, camera_up) * camRot;

   //Projection
   GLfloat aspect = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
   GLfloat fovy = field_of_view_y;
   GLfloat zNear = 0.1f;
   GLfloat zFar = 10.0f;
   glm::mat4 P = glm::perspective(fovy, aspect, zNear, zFar);
   
   //Other objects of the scene
   glm::vec4 light_position = glm::vec4(0.0f, 0.707f, 10.0f, 1.0f);
   glm::vec3 view = camera_center - camera_position;
   glm::vec3 light_direction = position - glm::vec3(light_position);


   //Activate modern OpenGL
   gl_program_ptr->use_program();
   
   //Geometric variables for the shader
   if (light_position_in_vs_loc != -1) {
	   glUniform4fv(light_position_in_vs_loc, 1, glm::value_ptr(V * light_position));
   }
   if (light_direction_in_vs_loc != -1) {
	   glUniform4fv(light_direction_in_vs_loc, 1, glm::value_ptr(V * glm::vec4(light_direction, 1.0f)));
   }
   if (view_vector_loc != -1) {
	   glUniform3fv(view_vector_loc, 1, glm::value_ptr(view));
   }
  
   if(P_loc != -1) {
      glUniformMatrix4fv(P_loc, 1, false, glm::value_ptr(P));
   }

   int pos_loc = gl_program_ptr->get_attrib_location("pos_attrib");
   int normal_loc = gl_program_ptr->get_attrib_location("normal_attrib");
   int tex_coord_loc = gl_program_ptr->get_attrib_location("tex_coord_attrib");

   //logics to draw the spheres
   if (switch_material_loc != -1) {
	   glUniform1i(switch_material_loc, 2);
   }
   std::vector<glm::vec3> positions = flag->get_positions();
   for (auto particle_pos : positions) {
	    glm::mat4 M;
		M = glm::translate(M, particle_pos);
		if (VM_loc != -1) {
	       glUniformMatrix4fv(VM_loc, 1, false, glm::value_ptr(V * M));
	    }
		glutSolidSphere(0.05, 10, 10);
   }

   //Logic to draw the edge
   if (switch_material_loc != -1) {
	   glUniform1i(switch_material_loc, 1);
   }
   std::vector<std::pair<glm::vec3, glm::vec3>> ends = flag->get_edges();
   for (auto end_points : ends) {
	   glm::vec3 a = end_points.first;
	   glm::vec3 b = end_points.second;
	   glm::vec3 a_to_b = glm::normalize(b - a);
	   //Glut cylinder born aligned to positive z-axis
	   glm::vec3 z_axis = glm::vec3(0.0f, 0.0f, 1.0f);
	   glm::vec3 axis = glm::normalize(glm::cross(z_axis, a_to_b));
	   GLfloat angle = glm::angle(z_axis, a_to_b);
	   
	   glm::mat4 M;
	   M = glm::translate(M, a);
	   M = glm::rotate(M, angle, axis);
	   //Tube born aligned to z axis
	   M = glm::scale(M, glm::vec3(0.025f, 0.025f, glm::distance(a, b)));
	   
	   if (VM_loc != -1) {
	       glUniformMatrix4fv(VM_loc, 1, false, glm::value_ptr(V * M));
	   }
	   
	   glutSolidCylinder(0.5f, 1.0f, 10, 20);
   }

   glutSwapBuffers();
   glUseProgram(0);
}

void init_open_gl() {
   GLenum error = glewInit();
   if (GLEW_OK != error) {
	   cerr << "Error: " << glewGetErrorString(error) << endl;
	   exit(EXIT_FAILURE);
   }
   cout << opengl::get_OpenGL_info();
   
   opengl::get_error_log();
   
   //Create program, load and compile shaders
   gl_program_ptr = new opengl::OpenGLProgram("shaders/PhongVertex.vert", "shaders/PhongFragment.frag");
   if (!gl_program_ptr->is_ok()) {
	   cerr << "Error at GL program creation" << endl;
	   opengl::gl_error();
	   exit(EXIT_FAILURE);
   }


   //Set some variables to shaders
   VM_loc = gl_program_ptr->get_uniform_location("VM");
   P_loc = gl_program_ptr->get_uniform_location("P");
   time_loc = gl_program_ptr->get_uniform_location("time");
   view_vector_loc = gl_program_ptr->get_uniform_location("view");
   light_position_in_vs_loc = gl_program_ptr->get_uniform_location("light_position_in_vs");
   light_direction_in_vs_loc = gl_program_ptr->get_uniform_location("light_direction_in_vs");

   //Make a location in the shader for program state variables
   texture_map_flag_loc = gl_program_ptr->get_uniform_location("texture_map_flag");
   light_mode_flag_loc = gl_program_ptr->get_uniform_location("light_mode");
   switch_material_loc = gl_program_ptr->get_uniform_location("switch_material");
   
   glUseProgram(0);

   glClearColor(0.15f, 0.15f, 0.15f, 0.0f);
   glEnable(GL_DEPTH_TEST);
}

void idle() {
	static int last_time = 0;

	int time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = time - last_time;
	last_time = time;
	float delta_seconds = 0.001f * elapsed;	
	seconds += delta_seconds;

	//Set shader time uniform variable
	gl_program_ptr->use_program();

   if(time_loc != -1) {
	   glUniform1f(time_loc, seconds);
   }
   flag->update();
  
   glutPostRedisplay();
}
	
void create_glut_window() {
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition (5, 5);
	glutInitWindowSize (640, 640);
	win = glutCreateWindow ("Flag simulator");
}

void create_glut_callbacks() {
	glutDisplayFunc(display);
	glutSpecialFunc(special_keys);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(mouse_wheel);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_active);
}

void exit_glut() {
	if (gl_program_ptr) {
		delete gl_program_ptr;
		gl_program_ptr = nullptr;
	}
	if (flag) {
		delete flag;
		flag = nullptr;
	}
	glutDestroyWindow(win);
	exit(EXIT_SUCCESS);
}

void init_program() {
	texture_map_flag = true;
	rotate_animation = false;
	texture_animation = true;
	seconds = 0.0;

	//Camera init
	field_of_view_y = TAU / 8.0f;
	camera_position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera_center = glm::vec3(0.0f, 0.0f, 0.0f);
	camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

	camera_pan = glm::vec2(0.0f, 0.0f);
	camera_base_rotation = glm::quat(1.0, glm::vec3(0.0f, 0.0f, 00.f));
	camera_new_rotation = glm::quat(1.0, glm::vec3(0.0f, 0.0f, 00.f));
	mode = NONE;

	flag = new Flag(8);
}

void bounding_box(glm::vec3* min, glm::vec3* max, float* mesh, int num_vertex) {
	if (num_vertex > 0) {
		assert(mesh != nullptr);
		assert(min != nullptr);
		assert(max != nullptr);
		*max = glm::vec3(mesh[0], mesh[1], mesh[2]);
		*min = glm::vec3(mesh[0], mesh[1], mesh[2]);
	   for (int i = 0; i < num_vertex * 3; i += 3) {
          if (mesh[i] > (*max)[0]) {
			  (*max)[0] = mesh[i];
		  }
		  if (mesh[i] < (*min)[0]) {
			  (*min)[0] = mesh[i];
		  }
		  if (mesh[i + 1] > (*max)[1]) {
			  (*max)[1] = mesh[i + 1];
		  }
		  if (mesh[i + 1] < (*min)[1]) {
			  (*min)[1] = mesh[i + 1];
		  }
		  if (mesh[i + 2] > (*max)[2]) {
			  (*max)[2] = mesh[i + 2];
		  }
		  if (mesh[i + 2] < (*min)[2]) {
			  (*min)[2] = mesh[i + 2];
		  }
       }
	}
	return;
}
