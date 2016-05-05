#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <glm/glm.hpp>

void reshape(int new_window_width, int new_window_height);
void keyboard(unsigned char key, int mouse_x, int mouse_y);
void special_keyboard(int key, int mouse_x, int mouse_y);
void mouse_active(int mouse_x, int mouse_y);
void mouse(int button, int state, int mouse_x, int mouse_y);
void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y);
void idle();

float projection_on_curve(glm::vec2 projected);
void reset_camera();

#endif