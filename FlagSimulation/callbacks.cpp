#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "globals.h"
#include "callbacks.h"


void mouse_active(int mouse_x, int mouse_y) {
	glm::vec2 mouse_current;
	if (mouse_dragging) {
		mouse_current = glm::vec2(static_cast<float>(mouse_x), static_cast<float>(mouse_y));
		glm::vec2 scale_factors = glm::vec2(2.0f / glutGet(GLUT_WINDOW_WIDTH), -2.0f / glutGet(GLUT_WINDOW_HEIGHT));
		if (mode == PAN) {
			glm::vec2 deltas = mouse_start_drag - mouse_current;
			camera_pan = scale_factors * deltas;
		}
		else if (mode == ROTATION) {
			
			glm::vec2 window_center = 0.5f * glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			glm::vec2 mouse_current_in_world = scale_factors * (mouse_current - window_center);
			glm::vec2 mouse_start_drag_in_world = scale_factors * (mouse_start_drag - window_center);
			
			glm::vec3 v_1 = glm::vec3(mouse_current_in_world, projection_on_curve(mouse_current_in_world));
			glm::vec3 v_2 = glm::vec3(mouse_start_drag_in_world, projection_on_curve(mouse_start_drag_in_world));
			v_1 = glm::normalize(v_1);
			v_2 = glm::normalize(v_2);
			glm::vec3 axis = glm::cross(v_1, v_2);
			float angle = glm::angle(v_1, v_2);
			camera_new_rotation = glm::normalize(glm::quat(glm::cos(0.5f * angle), glm::sin(0.5f * angle) * axis));
		}
	}
	glutPostRedisplay();
}

void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y) {
	const float DELTA_ANGLE = TAU / 20.0f;
	if (wheel == 0) {
		if (direction > 0 && field_of_view_y < ((TAU / 2.0f) - 2.0f * DELTA_ANGLE)) {
			field_of_view_y += DELTA_ANGLE;
		}
		else if (field_of_view_y > 2.0 * DELTA_ANGLE){
			field_of_view_y -= DELTA_ANGLE;
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
		}
		else if (button == GLUT_LEFT_BUTTON) {
			mode = ROTATION;
		}
	}
	else {
		mouse_dragging = false;
		if (button == GLUT_MIDDLE_BUTTON) {
			camera_center += glm::vec3(camera_pan, 0.0f);
			camera_position += glm::vec3(camera_pan, 0.0f);
			camera_pan = glm::vec2(0.0, 0.0);
		}
		else if (button == GLUT_LEFT_BUTTON) {
			/* Calculate the accumulated rotation: base rotation plus new one */
			camera_base_rotation = glm::normalize(camera_new_rotation * camera_base_rotation);
			/* Reset new rotation to identity */
			camera_new_rotation = glm::normalize(glm::quat(1.0f, glm::vec3(0.0f, 0.0f, 0.0f)));
		}
	}

	if (button == 3 || button == 4) {
		mouse_wheel(0, button == 3 ? 1 : -1, mouse_x, mouse_y);
	}

	glutPostRedisplay();
}

void special_keys(int key, int mouse_x, int mouse_y) {
	switch (key) {
		case GLUT_KEY_F3:
			fish_animation = (!fish_animation);
		break;

		case GLUT_KEY_F4:
			texture_map_flag = (!texture_map_flag);
		break;

		case GLUT_KEY_F5:
			light_mode_flag = (!light_mode_flag);
		break;

		case GLUT_KEY_F2:
			//rotate_animation = (!rotate_animation);
            rotate_animation = true;
			texture_animation = false;
		break;

		case GLUT_KEY_F1:
			//texture_animation = (!texture_animation);
			rotate_animation = false;
			texture_animation = true;
		break;
	}
	glutPostRedisplay();
}

void keyboard (unsigned char key, int mouse_x, int mouse_y) {
	switch (key) {

		case 'R':
		case 'r':
			field_of_view_y = TAU / 8.0f;
			camera_position = glm::vec3(0.0f, 0.0f, 5.0f);
			camera_center = glm::vec3(0.0f, 0.0f, 0.0f);
			camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

			camera_pan = glm::vec2(0.0f, 0.0f);
			camera_base_rotation = glm::quat(1.0, glm::vec3(0.0f, 0.0f, 00.f));
			camera_new_rotation = glm::quat(1.0, glm::vec3(0.0f, 0.0f, 00.f));
		break;

		case 'W':
		case 'w':
			flag->reset_flag();
		break;

	    case 27:
			exit(EXIT_SUCCESS);
		break;

		default:
		break;
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
