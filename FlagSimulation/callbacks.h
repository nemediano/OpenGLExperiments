#ifndef CALLBACKS_H_
#define CALLBACKS_H_

void special_keys(int key, int mouse_x, int mouse_y);
void keyboard (unsigned char key, int mouse_x, int mouse_y);
void mouse_active(int mouse_x, int mouse_y);
void mouse(int button, int state, int mouse_x, int mouse_y);
void mouse_wheel(int wheel, int direction, int mouse_x, int mouse_y);
float projection_on_curve(glm::vec2 projected);

#endif