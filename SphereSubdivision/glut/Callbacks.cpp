#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Globals.h"
#include "../opengl/HelperFunctions.h"
#include "../SphereSubdivision.h"
#include "Callbacks.h"
#include "../imgui/imgui_impl_glut.h"

void reshape(int new_window_width, int new_window_height) {
	glViewport(0, 0, new_window_width, new_window_height);
}

void idle() {
	//timers for time-based animation
	static int last_time = 0;
	int time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = time - last_time;
	options::delta_seconds = 0.001f * elapsed;
	last_time = time;

	opengl::gl_error("At idle"); //check for errors and print error strings
	glutPostRedisplay();
}

void keyboard(unsigned char key, int mouse_x, int mouse_y) {
	ImGui_ImplGlut_KeyCallback(key);
	if (key == 27) {//press ESC to exit
		exit_glut();
	} else if (key == 'c' || key == 'C') {
		reset_camera();
	} else if (key == 'w' || key == 'W') {
		options::wireframe = (!options::wireframe);
	} else if (key == 't' || key == 'T') {
		options::icosahedron = (!options::icosahedron);
		create_sphere();
		cout << "Starting solid: " << (options::icosahedron ? "icosahedron" : "tetrahedron") << endl;
	}

	glutPostRedisplay();
}

void keyboard_up(unsigned char key, int mouse_x, int mouse_y) {
	ImGui_ImplGlut_KeyUpCallback(key);
}

void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y) {
	const float DELTA_ANGLE = PI / 20.0f;
	if (wheel == 0) {
		if (direction > 0 && options::field_of_view_y < (PI - 2.0f * DELTA_ANGLE)) {
			options::field_of_view_y += DELTA_ANGLE;
		} else if (options::field_of_view_y > 2.0 * DELTA_ANGLE){
			options::field_of_view_y -= DELTA_ANGLE;
		}
	}
	glutPostRedisplay();
}

void mouse_active(int mouse_x, int mouse_y) {

	ImGui_ImplGlut_MouseMotionCallback(mouse_x, mouse_y);
	glm::vec2 mouse_current;
	if (options::mouse_dragging) {
		mouse_current = glm::vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
		glm::vec2 scale_factors = glm::vec2(2.0f / glutGet(GLUT_WINDOW_WIDTH), -2.0f / glutGet(GLUT_WINDOW_HEIGHT));	
		glm::vec2 window_center = 0.5f * glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		glm::vec2 mouse_current_in_world = scale_factors * (mouse_current - window_center);
		glm::vec2 mouse_start_drag_in_world = scale_factors * (options::mouse_start_drag - window_center);
		
		glm::vec3 v_1 = glm::vec3(mouse_current_in_world, projection_on_curve(mouse_current_in_world));
		glm::vec3 v_2 = glm::vec3(mouse_start_drag_in_world, projection_on_curve(mouse_start_drag_in_world));
		v_1 = glm::normalize(v_1);
		v_2 = glm::normalize(v_2);
		glm::vec3 axis = glm::cross(v_1, v_2);
		float angle = glm::angle(v_1, v_2);
		options::camera_new_rotation = glm::normalize(glm::quat(glm::cos(0.5f * angle), glm::sin(0.5f * angle) * axis));
		
	}
	glutPostRedisplay();
}

void mouse_motion(int mouse_x, int mouse_y) {
	ImGui_ImplGlut_MouseMotionCallback(mouse_x, mouse_y);
}

void mouse_passive(int mouse_x, int mouse_y) {
	ImGui_ImplGlut_PassiveMouseMotionCallback(mouse_x, mouse_y);
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

void mouse(int button, int state, int mouse_x, int mouse_y) {
	ImGui_ImplGlut_MouseButtonCallback(button, state);
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		options::mouse_dragging = true;
		options::mouse_start_drag.x = static_cast<float>(mouse_x);
		options::mouse_start_drag.y = static_cast<float>(mouse_y);
	}
	else if (button == GLUT_LEFT_BUTTON) {
		options::mouse_dragging = false;
		/* Calculate the accumulated rotation: base rotation plus new one */
		options::camera_base_rotation = glm::normalize(options::camera_new_rotation * options::camera_base_rotation);
		/* Reset new rotation to identity */
		options::camera_new_rotation = glm::normalize(glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f)));
	}
	
	glutPostRedisplay();
}

void special_keyboard(int key, int mouse_x, int mouse_y) {
	ImGui_ImplGlut_SpecialCallback(key);
	using namespace std;
	switch (key) {
		case GLUT_KEY_UP:
		{
			options::subdiv_level++;
			cout << "Subdivision level: " << options::subdiv_level << endl;
			create_sphere();
		}
		break;

		case GLUT_KEY_DOWN:
		{
			options::subdiv_level = glm::max(0, --options::subdiv_level);
			cout << "Subdivision level: " << options::subdiv_level << endl;
			create_sphere();
		}
		break;

	}
	glutPostRedisplay();
}

void special_keybpard_up(int key, int mouse_x, int mouse_y) {
	ImGui_ImplGlut_SpecialUpCallback(key);
}

void reset_camera() {
	options::camera_position = glm::vec3(0.0f, 0.0f, options::world_radious);
	options::camera_center = glm::vec3(0.0f, 0.0f, 0.0f);
	options::camera_up = glm::vec3(0.0f, 1.0f, 0.0);
	options::field_of_view_y = PI / 3.0f;
	options::camera_base_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	options::camera_new_rotation = glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	options::camera_pan = glm::vec2(0.0, 0.0);
}